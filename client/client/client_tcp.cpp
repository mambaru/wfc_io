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
}

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
    _impl->start( opt );
  }
  else
  {
    domain_object::start(arg);
  }
}

void client_tcp::reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/)
{
  DEBUG_LOG_MESSAGE("client_tcp::reg_io")
  // не регистрируем нандлер (будет авторегистрация при запросе)
}

void client_tcp::unreg_io(io_id_t io_id)
{
  // Обязательно удаляем обработчик при закрыии источника
  _impl->erase_handler(io_id);
}

void client_tcp::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
{
  IOW_LOG_DEBUG(" DEBUG: --- client_tcp::perform_io --- ######## 1 ##########")
  _impl->send( std::move(d), io_id, std::move(handler) );
  IOW_LOG_DEBUG(" DEBUG: --- client_tcp::perform_io --- ######## 2!!! END ##########")

}

}
