#include <iostream>
#include "server_tcp.hpp"
#include "tcp_acceptor.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/server/server.hpp>


namespace wfc{ namespace io{
  
typedef iinterface::io_id_t io_id_t;
typedef iinterface::outgoing_handler_t outgoing_handler_t;
typedef iinterface::data_ptr data_ptr;


class server_tcp::impl
  : public ::iow::ip::tcp::server::server<tcp_acceptor>
{
public:
  typedef server::io_service_type io_service_type;
  impl(io_service_type& io)
    : server(io)
  {}
};


server_tcp::~server_tcp()
{
}

server_tcp::server_tcp()
{
}

void server_tcp::initialize()
{
  if ( auto g = this->global() )
  {
    auto target = this->options().target;
    _target = g->registry.get<iinterface>(target);
  }

}

void server_tcp::start(const std::string& arg)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<impl>( g->io_service );
    auto opt = this->options();

    auto wtarget = _target;

    /// #warning убрать, но в базовых не должно nonblocking, т.к. nonblocking() для акцепт вылетает сразу 
    opt.nonblocking = false;

    if ( opt.keep_alive ) 
    {
      opt.connection.incoming_handler = 
        [wtarget]( data_ptr d, io_id_t id, outgoing_handler_t callback ) 
      {
        if ( auto ptarget = wtarget.lock() )
        {
          ptarget->perform_io(std::move(d), id, std::move(callback));
        }
        else
        {
          callback( std::move(d));
        }
      };
    }
    else
    {
      opt.connection.incoming_handler = 
        [wtarget](data_ptr d, io_id_t id, outgoing_handler_t callback )
      {
        if ( auto ptarget = wtarget.lock() )
        {
          ptarget->perform_io(std::move(d), id, [callback](data_ptr d)
          {
            callback(std::move(d));
            callback(nullptr);
          });
        }
        else
        {
          callback( std::move(d));
        }
      };
    }
    opt.connection.target = wtarget;
    
    /*
#warning сделать отключчаемеми
    
    opt.connection.startup_handler = [wtarget]( ::iow::io::io_id_t id, ::iow::io::outgoing_handler_t callback)
    {
      if ( auto ptarget = wtarget.lock() )
      {
        DEBUG_LOG_MESSAGE("wfc_server::connection reg_io")
        //ptarget->reg_io( id, wthis);  
      }
    };
    */
    
    
    //_impl->start( opt );
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

}}
