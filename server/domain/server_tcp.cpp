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

void server_tcp::start(const std::string& arg)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<server_tcp_impl>( g->io_service );
    auto opt = this->options();
    opt.host = "0.0.0.0";
    opt.port = "12345";
    _impl->start( std::move(opt) );
  }
  else
  {
    domain_object::start(arg);
  }
}

}
