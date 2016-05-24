#include <iostream>
#include "client_tcp.hpp"
#include "client_tcp_map.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/asio.hpp> // TODO: убрать 
#include <wfc/memory.hpp>

namespace wfc{
 

client_tcp::~client_tcp()
{
}

client_tcp::client_tcp()
{
}

void client_tcp::reconfigure()
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<client_tcp_map>( g->io_service);
    auto opt = this->options();
    _workflow = ::wfc::workflow::recreate_and_start(_workflow, opt.workflow_opt);
    opt.args.workflow = _workflow->get();
    _impl->reconfigure( opt );
  }
  else
  {
    domain_object::reconfigure();
  }
}

void client_tcp::start(const std::string&) 
{
  
}

void client_tcp::stop(const std::string&) 
{
  if ( _impl!=nullptr )
  {
    _impl->stop();
  }
}

void client_tcp::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  this->_impl->reg_io( io_id, itf);
}

void client_tcp::unreg_io(io_id_t io_id)
{
  _impl->unreg_io(io_id);
}

void client_tcp::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
{
  _impl->perform_io( std::move(d), io_id, std::move(handler) );
}

}
