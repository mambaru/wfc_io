#include <iostream>
#include "server_tcp.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/server/server.hpp>
namespace wfc{

class server_tcp_impl
  : public ::iow::ip::tcp::server::server<>
{
public:
  typedef server::io_service_type io_service_type;
  server_tcp_impl(io_service_type& io)
    : server(io)
  {}
};

server_tcp::~server_tcp()
{
}

server_tcp::server_tcp()
{
}

void server_tcp::reconfigure()
{
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
}

void server_tcp::start(const std::string& arg)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<server_tcp_impl>( g->io_service );
    auto opt = this->options();
    /*
    opt.host = "0.0.0.0";
    opt.port = "12345";
    opt.connection_options.reader.sep = "\r\n";
    opt.connection_options.reader.trimsep = true;
    opt.connection_options.writer.sep = "\r\n";
    opt.connection_options.reader.bufsize = 512;
    */
    
    auto wtarget = _target;
    
#warning убрать, но в базовых не должно nonblocking, т.к. nonblocking() для акцепт вылетает сразу 
    opt.nonblocking = false;
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

void server_tcp::stop(const std::string&) 
{
  if ( _impl != nullptr )
  {
    CONFIG_LOG_BEGIN("Server stop")
    _impl->stop();
    CONFIG_LOG_END("Server stop")
    _impl=nullptr;
  }
}

}
