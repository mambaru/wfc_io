
#include "client_tcp_adapter.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <iow/io/io_id.hpp>


namespace wfc{

class client_tcp_adapter::impl
  : public ::iow::ip::tcp::client::thread<>
{
  typedef ::iow::ip::tcp::client::thread<> super;
public:
  impl( io_service_type& io)
    : super(io){}
};

client_tcp_adapter::client_tcp_adapter( io_service_type& io, std::weak_ptr<iinterface> holder)
  : _id ( ::iow::io::create_id<io_id_t>() )
  , _holder(holder)
{
  if (holder.lock()==nullptr) abort();
  _client = std::make_shared<client_type>(io);
}

client_tcp_adapter::~client_tcp_adapter() 
{
  _holder.lock()->unreg_io(_id);
}

void client_tcp_adapter::stop()
{
  _client->stop();
}

void client_tcp_adapter::start( options_type opt)
{
  auto pthis = this->shared_from_this();
  
  if ( _holder.lock() != nullptr )
  {
    if ( opt.connection.incoming_handler == nullptr )
    {
      opt.connection.incoming_handler = [pthis](data_ptr d, io_id_t, outgoing_handler_t handler)
      {
        if ( auto holder = pthis->_holder.lock() )
        {
          holder->perform_io(std::move(d), pthis->_id, std::move(handler) );
        }
      };
    }

    auto connect_handler = opt.connect_handler;
    opt.connect_handler = [pthis, connect_handler]()
    {
      if ( auto holder = pthis->_holder.lock() )
      {
        holder->reg_io( pthis->_id, pthis );
      }
      if ( connect_handler ) connect_handler();
    };
    
    auto shutdown_handler = opt.connection.shutdown_handler;
    opt.connection.shutdown_handler = [pthis, shutdown_handler](io_id_t id)
    {
      if ( auto holder = pthis->_holder.lock() )
        holder->unreg_io( pthis->_id);
      if ( shutdown_handler ) shutdown_handler( id );
    };
  }
  else
  {
    IOW_LOG_WARNING("client_tcp::client::start holder not set")
  }
  
  _client->start(opt);
}

std::weak_ptr<iinterface> client_tcp_adapter::get_holder() const
{
  return _holder;
}

void client_tcp_adapter::set_holder(std::weak_ptr<iinterface> holder)
{
  _holder = holder;
}

// iinterface

void client_tcp_adapter::perform_io( iinterface::data_ptr d, io_id_t /*io_id*/, outgoing_handler_t handler) 
{
  if ( auto rd = _client->send( std::move(d) ) )
  {
    DEBUG_LOG_ERROR("tcp_client send FAIL: " << d)
    if (handler!=nullptr)
      handler( nullptr );
  }
}


}
