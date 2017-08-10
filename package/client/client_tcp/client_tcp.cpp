#include <iostream>
#include "client_tcp.hpp"
#include "client_tcp_adapter.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/asio.hpp> // TODO: убрать 
#include <wfc/memory.hpp>

namespace wfc{ namespace io{
 

void client_tcp::start()
{
  if ( _adapter!=nullptr )
    _adapter->stop();
  _adapter = std::make_shared<client_tcp_adapter>( this->global()->io_service);
  auto opt = this->options();
  opt.args.workflow = this->get_workflow();
  _adapter->start( opt );
}

void client_tcp::stop() 
{
  if ( _adapter!=nullptr )
  {
    _adapter->stop();
  }
}

void client_tcp::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  if ( !this->suspended() && _adapter!=nullptr )
  {
    _adapter->reg_io( io_id, itf);
  }
}

void client_tcp::unreg_io(io_id_t io_id)
{
  if ( _adapter!=nullptr )
  {
    _adapter->unreg_io(io_id);
  }
}

void client_tcp::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) 
{
  if ( this->suspended() )
  {
    if ( handler!= nullptr )
      handler(nullptr);
  }
  else if ( _adapter!=nullptr )
  {
    _adapter->perform_io( std::move(d), io_id, std::move(handler) );
  }
}

}}
