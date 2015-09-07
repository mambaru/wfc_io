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

void client_tcp::start(const std::string& arg)
{
  
  if ( auto g = this->global() )
  {

    _impl = std::make_shared<client_tcp_impl>( g->io_service );
    auto opt = this->options();

    /*opt.addr = "0.0.0.0";
    opt.port = "12345";
    opt.connection.reader.sep = "\r\n";
    opt.connection.reader.trimsep = true;
    opt.connection.writer.sep = "\r\n";
    opt.connection.reader.bufsize = 512;
    */
    
    opt.connection.startup_handler=[]( ::iow::io::io_id_t, ::iow::io::outgoing_handler_t outgoing){
      DEBUG_LOG_MESSAGE("Connected!!!");
      g_tmp = outgoing;
    };
    
    auto wtarget = _target;
    opt.connection.incoming_handler = [wtarget]( 
      std::unique_ptr< std::vector<char> > d,
      size_t id,
      std::function<void(std::unique_ptr< std::vector<char> >)> callback
    )
    {
      if ( auto ptarget = wtarget.lock() )
      {
        ptarget->perform_io(std::move(d), id, callback);
      }
      else
      {
        callback( std::move(d));
      }
    };
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

void client_tcp::perform_io(data_ptr d, io_id_t /*io_id*/, outgoing_handler_t /*handler*/) 
{
  DEBUG_LOG_BEGIN("client_tcp::perform_io")
  
  // _impl->write( std::move(d) /*, std::move(handler)*/ );
  g_tmp( std::move(d) );
  
  DEBUG_LOG_END("client_tcp::perform_io")
  
}


}
