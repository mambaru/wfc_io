#include <iostream>
#include "server_tcp.hpp"
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
    _target = g->registry.get<iinterface>(this->options().target);
  }
}

void server_tcp::start(const std::string& arg)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<server_tcp_impl>( g->io_service );
    auto opt = this->options();
    opt.host = "0.0.0.0";
    opt.port = "12345";
    opt.connection_options.reader.sep = "\r\n";
    
    auto wtarget = _target;
    opt.connection_options.incoming_handler = [wtarget]( 
      std::unique_ptr< std::vector<char> > d,
      size_t id,
      std::function<void(std::unique_ptr< std::vector<char> >)> callback
    )
    {
      std::cout << "=====> callback [" << std::string( d->begin(), d->end() ) << std::endl;
      if ( auto ptarget = wtarget.lock() )
      {
        std::cout << "TARGET READY"  << std::endl;
      }
      else
      {
        std::cout << "NO TARGET"  << std::endl;
      }
      callback( std::move(d));
    };
    _impl->start( std::move(opt) );
  }
  else
  {
    domain_object::start(arg);
  }
}

}
