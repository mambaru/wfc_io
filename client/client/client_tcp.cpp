#include <iostream>
#include "client_tcp.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <wfc/asio.hpp> // TODO: убрать 

namespace wfc{

class client_tcp::impl
   : public ::iow::ip::tcp::client::multi_client<>
{
public:
  typedef ::iow::ip::tcp::client::multi_client<> super;
  typedef super::io_service_type io_service_type; // TODO: вернуть
  
  impl( io_service_type& io)
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

/*
::iow::io::outgoing_handler_t g_tmp = nullptr;
::iow::io::outgoing_handler_t g_tmp2 = nullptr;
*/

void client_tcp::stop(const std::string&) 
{
  if ( _impl!=nullptr )
  {
    _impl->stop();
  }
}

void client_tcp::start(const std::string& arg)
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<client_tcp::impl>( g->io_service );
    auto opt = this->options();
    
    /*
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
    */

    /*
    opt.connection.outgoing_handler = []( ::iow::io::data_ptr d)
    {
      g_tmp2( std::move(d) );
    };*/
    _impl->start( opt );
  }
  else
  {
    domain_object::start(arg);
  }
}

void client_tcp::reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> itf)
{
  DEBUG_LOG_MESSAGE("client_tcp::reg_io")
  // не регистрируем нандлер (будет авторегистрация при запросе)
}

void client_tcp::unreg_io(io_id_t io_id)
{
  DEBUG_LOG_MESSAGE("client_tcp::unreg_io")
  // Обязательно удаляем обработчик при закрыии источника
  _impl->erase_handler(io_id);
}

void client_tcp::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
{
  DEBUG_LOG_MESSAGE("client_tcp::perform_io -1- io_id=" << io_id << "[" << d << "]")
  _impl->send( std::move(d), io_id, std::move(handler) );
  DEBUG_LOG_MESSAGE("client_tcp::perform_io -2-")
  //_impl->send( std::move(d) );
}


}
