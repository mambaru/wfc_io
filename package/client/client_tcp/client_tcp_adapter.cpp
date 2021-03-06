
#include "client_tcp_adapter.hpp"
#include <wfc/logger.hpp>
#include <wfc/wfc_exit.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/mutex.hpp>

namespace wfc{ namespace io{

class client_tcp_adapter::handler_wrapper: public iinterface
{
public:
  explicit handler_wrapper(output_handler_t handler): _handler(handler) {}
  virtual void perform_io( iinterface::data_ptr d, io_id_t /*id*/, output_handler_t /*handler*/) override
  {
    _handler( std::move(d) );
  }
private:
  output_handler_t _handler;
};

class client_tcp_adapter::impl
  : public ::iow::ip::tcp::client::multi_thread<>
{
  typedef ::iow::ip::tcp::client::multi_thread<> super;
public:
  explicit impl( io_context_type& io)
    : super(io){}
};

client_tcp_adapter::client_tcp_adapter( io_context_type& io)
  : _io (io)
  , _id ( ::iow::io::create_id<io_id_t>() )
{
  _holder_id = 0;
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
  auto pthis = this->shared_from_this();

  if ( opt.connection.input_handler == nullptr )
  {
    opt.connection.input_handler = [pthis](data_ptr d, io_id_t, output_handler_t handler)
    {
      if ( auto holder = pthis->get_holder() )
      {
        holder->perform_io(std::move(d), pthis->_id, std::move(handler) );
      }
    };
  }

  auto connect_handler = opt.args.connect_handler;
  opt.args.connect_handler = [pthis, connect_handler]()
  {
    if ( auto holder = pthis->get_holder() )
    {
      holder->reg_io( pthis->_id, pthis );
    }
    if ( connect_handler!=nullptr )
      connect_handler();
  };

  auto shutdown_handler = opt.connection.shutdown_handler;
  opt.connection.shutdown_handler = [pthis, shutdown_handler](io_id_t id)
  {
    if ( auto holder = pthis->get_holder() )
    {
      auto this_id = pthis->_id;
      boost::asio::post(pthis->_io, [holder, this_id]()
      {
        holder->unreg_io(this_id);
      }/*, nullptr*/);

    }
    if ( shutdown_handler != nullptr )
      shutdown_handler( id );
  };

  try
  {
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
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _holder_id > 0 && _holder_id!=io_id  )
  {
    DOMAIN_LOG_FATAL("client-tcp configuration error! Several sources are unacceptable _holder_id=" << _holder_id <<", io_id="<< io_id)
  }
  _holder_id = io_id;
  _holder = itf;
  _wrapper = nullptr;
}

void client_tcp_adapter::unreg_io(io_id_t io_id)
{
  std::lock_guard<mutex_type> lk(_mutex);

  if ( _holder_id!=io_id )
    return;

  _holder_id = 0;
  _wrapper = nullptr;
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
        _wrapper = std::make_shared<handler_wrapper>(handler);
        _holder = _wrapper;
        _holder_id = io_id;
      }
    }
  }

  if ( auto rd = _client->send( std::move(d) ) )
  {
    if (handler!=nullptr)
    {
      handler( nullptr );
    }
  }
}

}}
