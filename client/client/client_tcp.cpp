#include <iostream>
#include "client_tcp.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/client/client.hpp>


#include <wfc/asio.hpp> // TODO: убрать 
namespace wfc{

  /*
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
*/
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

void client_tcp::start(const std::string& arg)
{
  if ( auto g = this->global() )
  {
    /*
    _impl = std::make_shared<client_tcp_impl>( g->io_service );
    auto opt = this->options();
    */
    /*
    opt.host = "0.0.0.0";
    opt.port = "12345";
    opt.connection_options.reader.sep = "\r\n";
    opt.connection_options.reader.trimsep = true;
    opt.connection_options.writer.sep = "\r\n";
    opt.connection_options.reader.bufsize = 512;
    */
    
    /*
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
    */
  }
  else
  {
    domain_object::start(arg);
  }
}

}
