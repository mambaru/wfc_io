
#include "client_tcp_adapter.hpp"
#include <wfc/logger.hpp>
#include <wfc/wfc_exit.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/mutex.hpp>

namespace wfc{ namespace io{

class client_tcp_adapter::perform_holder: public iinterface
{
public:
  explicit perform_holder(output_handler_t handler): _handler(handler) {}
  virtual void perform_io( iinterface::data_ptr d, io_id_t /*id*/, output_handler_t /*handler*/) override
  {
    _handler( std::move(d) );
  }
private:
  output_handler_t _handler;
};

class client_tcp_adapter::impl
  : public ::iow::ip::tcp::client::multi_client<>
{
  typedef ::iow::ip::tcp::client::multi_client<> super;
public:
  explicit impl( io_context_type& io)
    : super(io){}
};

client_tcp_adapter::client_tcp_adapter( io_context_type& io)
  : _io (io)
  , _id ( ::iow::io::create_id<io_id_t>() )
{
  _holder_id = 0;
  _error_flag = false;
  _client = std::make_shared<client_type>(io);
}

client_tcp_adapter::~client_tcp_adapter()
{
  if ( auto h = _holder.lock() )
  {
    h->unreg_io(_id);
  }
}

void client_tcp_adapter::stop()
{
  _client->stop();
}

void client_tcp_adapter::start( options_type opt)
{
  std::weak_ptr<client_tcp_adapter> wthis = this->shared_from_this();
  
  _keep_alive = opt.keep_alive;
  if ( opt.connection.input_handler == nullptr )
  {
    opt.connection.input_handler = [wthis](data_ptr d, io_id_t, output_handler_t handler)
    {
      if ( auto pthis = wthis.lock() )
      {
        if ( auto holder = pthis->get_holder() )
        {
          if ( pthis->_keep_alive )
          {
            holder->perform_io(std::move(d), pthis->_id, std::move(handler) );
          }
          else
          {
            if ( handler!=nullptr ) 
              handler(nullptr);
            holder->perform_io(std::move(d), pthis->_id, [wthis](data_ptr dd)
            {
              // Для случая когда клиент в !keep_alive и получил встречный запрос с сервера
              // через этот обработчик можно отправить ответ по другому соединению 
              if ( auto pthis2 = wthis.lock() )
                pthis2->perform_io(std::move(dd), pthis2->_holder_id, nullptr );
            });
          }
        }
      }
    };
  }

  auto connect_handler = opt.args.connect_handler;
  opt.args.connect_handler = [wthis, connect_handler]()
  {
    if ( auto pthis = wthis.lock() )
    {
      if ( auto holder = pthis->get_holder() )
      {
        holder->reg_io( pthis->_id, pthis );
      }
      if ( connect_handler!=nullptr )
        connect_handler();
    }
  };

  auto shutdown_handler = opt.connection.shutdown_handler;
  opt.connection.shutdown_handler = [wthis, shutdown_handler](io_id_t id)
  {
    if ( auto pthis = wthis.lock() )
    {
      if ( auto holder = pthis->get_holder() )
      {
        auto this_id = pthis->_id;
        boost::asio::post(pthis->_io, [holder, this_id]()
        {
          holder->unreg_io(this_id);
        });

      }
      if ( shutdown_handler != nullptr )
        shutdown_handler( id );
    }
  };

  try
  {
    _options = opt;
    _client->start(opt);
  }
  catch(const std::exception& e)
  {
    if ( opt.abort_if_error )
    {
      DOMAIN_LOG_FATAL( "client-tcp: " << opt.port << " error: " << e.what() )
    }
    else
    {
      DOMAIN_LOG_ERROR( "client-tcp: " << opt.port << " error: " << e.what() )
    }
  }
}

std::shared_ptr<iinterface> client_tcp_adapter::get_holder() const
{
  read_lock<mutex_type> lk(_mutex);
  return _holder.lock();
}

// iinterface

void client_tcp_adapter::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  std::weak_ptr<iinterface> wholder;
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _holder_id > 0 && _holder_id!=io_id  )
    {
      DOMAIN_LOG_FATAL("client-tcp logical error! Several sources are unacceptable _holder_id=" 
                       << _holder_id <<", io_id="<< io_id)
    }
    _holder_id = io_id;
    _holder = itf;
    _perform_holder = nullptr;
    wholder = _holder;
  }
  
  if ( auto pholder = wholder.lock() )
    pholder->reg_io( _id, this->shared_from_this() );
}

void client_tcp_adapter::unreg_io(io_id_t io_id)
{
  std::lock_guard<mutex_type> lk(_mutex);

  if ( _holder_id!=io_id )
    return;

  _holder_id = 0;
  _perform_holder = nullptr;
}


void client_tcp_adapter::perform_io( iinterface::data_ptr d, io_id_t io_id, output_handler_t handler)
{
  auto pitf = this->get_holder();
  if ( pitf == nullptr || io_id==0)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( _holder.lock() == nullptr || io_id==0)
    {
      if ( handler!=nullptr || io_id==0 )
      {
        DOMAIN_LOG_WARNING("The object sent data without registration (did not call reg_io). "
                           "A wrapper based on the callback function has been created for it. "
                           "This will probably work correctly, but to avoid side effects, "
                           "the object sending the data should call the reg_io method")
        
        _perform_holder = std::make_shared<perform_holder>(handler);
        _holder = _perform_holder;
        _holder_id = io_id;
      }
    }
  }

  if ( auto rd1 = _client->send( std::move(d) ) )
  {
    // Попытка создать новое подключение если установленая опция connect_by_request
    if ( auto rd2 = _client->send( std::move(rd1), _options ) )
    {
      if ( !_error_flag )
      {
        _error_flag = true;
        IOW_LOG_ERROR("client-tcp: No suitable client to send data was found. Not send: [" << rd2 << "]")
      }
      if (handler!=nullptr)
        handler( nullptr );
    }
    else
      _error_flag = false;
  }
  else
    _error_flag = false;
}

}}
