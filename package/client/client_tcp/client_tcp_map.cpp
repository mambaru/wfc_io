#include "client_tcp_map.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

#error сделать пул подключений и рекконект для них 
#error отработать рекконект когда пришли данные
  
client_tcp_map::client_tcp_map( io_service_type& io)
  : _io(io)
{
}

void client_tcp_map::reconfigure(const options_type& opt)
{
  std::lock_guard<mutex_type> lk(_mutex);
  _opt = opt;
  for ( auto& item : _clients )
  {
    auto cli = item.second;
    cli->stop();
    item.second = std::make_shared<client_type>(_io);
    cli->start(opt);
  }
}

void client_tcp_map::stop()
{
  for ( auto& cli : _clients )
  {
    cli.second->stop();
    cli.second = nullptr;
  }
  _clients.clear();
}

client_tcp_map::client_ptr client_tcp_map::find( io_id_t id ) const
{
  read_lock<mutex_type> lk(_mutex);
  return this->find_(id);
}

client_tcp_map::client_ptr client_tcp_map::upsert(io_id_t id)
{
  {
    read_lock<mutex_type> lk(_mutex);
    if ( client_ptr cli = this->find_(id) )
      return cli;
  }
  std::lock_guard<mutex_type> lk(_mutex);
  client_ptr cli = std::make_shared<client_type>(_io);
  _clients.insert( std::make_pair(id, cli ) );
  cli->start(_opt);
  return cli;
}

// iinterface

void client_tcp_map::reg_io( io_id_t id, std::weak_ptr<iinterface> holder)
{
  if ( client_ptr cli = this->upsert(id) )
  {
    cli->reg_io(id, holder);
  }
}

void client_tcp_map::unreg_io( io_id_t id)
{
  if ( client_ptr cli = this->find(id) )
  {
    cli->unreg_io(id);
    cli->stop();

    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _clients.find(id);
    if ( itr == _clients.end() )
      return;
    _clients.erase(itr);
  }
}

void client_tcp_map::perform_io( data_ptr d, io_id_t id, output_handler_t handler)
{
  // Для клиента reg_io обязателен 
  if ( auto cli = this->find(id ) )
  {
    cli->perform_io( std::move(d), id, std::move(handler) );
  }
  else if ( handler != nullptr )
  {
    handler(nullptr);
  }
}

// private

client_tcp_map::client_ptr client_tcp_map::find_( io_id_t id ) const
{
  auto itr = _clients.find(id);
  if ( itr!=_clients.end() )
    return itr->second;
  return nullptr;
}

}}
