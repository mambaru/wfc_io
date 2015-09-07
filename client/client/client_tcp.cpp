#include <iostream>
#include "client_tcp.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <wfc/asio.hpp> // TODO: убрать 

namespace wfc{

class client_tcp_impl
   : public ::iow::ip::tcp::client::client<>
{
public:
  typedef ::iow::ip::tcp::client::client<> super;
  typedef super::io_service_type io_service_type; // TODO: вернуть
  
  client_tcp_impl( io_service_type& io)
    : super(io)
  {}
};

client_tcp::~client_tcp()
{
}

client_tcp::client_tcp()
{
}

void client_tcp::reconfigure()
{
  /*
  if ( auto g = this->global() )
  {
    auto target = this->options().target;
    _target = g->registry.get<iinterface>(target);
    if ( _target.lock() != nullptr)
    {
      CONFIG_LOG_MESSAGE("Target '" << target << "' found" )
    }
    else
    {
      CONFIG_LOG_WARNING("Target '" << target << "' NOT found" )
    }
  }
  */
}

::iow::io::outgoing_handler_t g_tmp = nullptr;
::iow::io::outgoing_handler_t g_tmp2 = nullptr;

void client_tcp::start(const std::string& arg)
{
  
  if ( auto g = this->global() )
  {

    _impl = std::make_shared<client_tcp_impl>( g->io_service );
    auto opt = this->options();
    
    opt.connection.startup_handler=[]( ::iow::io::io_id_t, ::iow::io::outgoing_handler_t outgoing){
      DEBUG_LOG_MESSAGE("Connected!!!");
      g_tmp = outgoing;
    };
    
    auto wtarget = _target;
    opt.connection.incoming_handler = [wtarget]( 
      ::iow::io::data_ptr d,
      size_t id,
      std::function<void(::iow::io::data_ptr)> callback
    )
    {
      DEBUG_LOG_MESSAGE("client_tcp: " << d )
      if ( auto ptarget = wtarget.lock() )
      {
        DEBUG_LOG_MESSAGE("client_tcp -1- " )
        ptarget->perform_io(std::move(d), id, callback);
      }
      else
      {
        DEBUG_LOG_MESSAGE("client_tcp -2- id=" << id )
        //callback( std::move(d));
        g_tmp2( std::move(d) );
      }
    };

    /*
    opt.connection.outgoing_handler = []( ::iow::io::data_ptr d)
    {
      g_tmp2( std::move(d) );
    };*/
    _impl->start( std::move(opt) );
  }
  else
  {
    domain_object::start(arg);
  }
}

void client_tcp::reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/)
{
  DEBUG_LOG_MESSAGE("client_tcp::reg_io")
}

void client_tcp::unreg_io(io_id_t /*io_id*/)
{
  DEBUG_LOG_MESSAGE("client_tcp::unreg_io")
}

void client_tcp::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
{
  DEBUG_LOG_BEGIN("client_tcp::perform_io" << io_id )
  g_tmp2 = handler;
  // _impl->write( std::move(d) /*, std::move(handler)*/ );
  g_tmp( std::move(d) );
  
  DEBUG_LOG_END("client_tcp::perform_io")
  
}


}
