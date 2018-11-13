#include "client_tcp_map.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

class client_tcp_map::handler_wrapper: public iinterface
{
public:
  explicit handler_wrapper(output_handler_t handler): _handler(handler) {}
  virtual void perform_io( iinterface::data_ptr d, io_id_t /*id*/, output_handler_t /*handler*/) override
  {
    _handler( std::move(d) );
  }
private:
  output_handler_t _handler;
};

client_tcp_map::client_tcp_map( io_service_type& io)
  : _io(io)
{
}

void client_tcp_map::reconfigure(const options_type& opt)
{
  client_list_t client_list;
  
  {
    read_lock<mutex_type> lk(_mutex);
    for ( auto& item : _clients )
      client_list.push_back(item.second);
    for ( auto& cli : _primary_pool )
      client_list.push_back(cli);
    for ( auto& cli : _secondary_pool)
      client_list.push_back(cli);
    for ( auto& cli : _startup_pool )
      client_list.push_back(cli);
  }
  
  for ( auto& cli : client_list )
    cli->stop();
  client_list.clear();
  
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _opt = opt;
    auto shutdown_handler = opt.connection.shutdown_handler;
    _opt.connection.shutdown_handler = [shutdown_handler](io_id_t id)
    {
      if ( shutdown_handler!=nullptr )
        shutdown_handler(id);
    };
  
    for ( auto& item : _clients )
    {
      item.second = std::make_shared<client_type>(_io);
      client_list.push_back(item.second);
      item.second->start(opt);
    }

    _secondary_pool.clear();
    
    _primary_pool.clear();
    while ( _primary_pool.size() < opt.primary_pool )
    {
      auto cli = std::make_shared<client_type>(_io);
      _primary_pool.push_back(cli);
      client_list.push_back(cli);
    }
    
    _startup_pool.clear();
    if ( _startup_flag )
    {
      while ( _startup_pool.size() < opt.startup_pool )
      {
        auto cli = std::make_shared<client_type>(_io);
        _startup_pool.push_back(cli);
        client_list.push_back(cli);
      }
      
      _startup_flag = false;
    }
  }
  
  for ( auto& cli : client_list )
    cli->start(_opt);
  
  _stop_flag = false;
}

void client_tcp_map::stop()
{
  _stop_flag = true;
  client_list_t client_list;
  {
    read_lock<mutex_type> lk(_mutex);
    for ( auto& item : _clients )
      client_list.push_back(item.second);
    for ( auto& cli : _primary_pool )
      client_list.push_back(cli);
    for ( auto& cli : _secondary_pool)
      client_list.push_back(cli);
    for ( auto& cli : _startup_pool )
      client_list.push_back(cli);
    _clients.clear();
    _primary_pool.clear();
    _secondary_pool.clear();
    _startup_pool.clear();
  }
  
  
  for ( auto& cli : client_list )
    cli->stop();
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
  
  auto cli = this->create_();
  std::lock_guard<mutex_type> lk(_mutex);
  _clients.insert( std::make_pair(id,  cli) );
  return cli;
}

// iinterface

void client_tcp_map::reg_io( io_id_t id, std::weak_ptr<iinterface> holder)
{
  if ( _stop_flag ) return;
  if ( client_ptr cli = this->upsert(id) )
  {
    cli->reg_io(id, holder);
  }
}

void client_tcp_map::unreg_io( io_id_t id)
{
  if ( _stop_flag ) return;
  if ( client_ptr cli = this->find(id) )
  {
    cli->unreg_io(id);
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _clients.erase(id);
      if ( _secondary_pool.size() < _opt.secondary_pool )
        _secondary_pool.push_back( std::move(cli) );
    }
    
    // Если  не перенесен в пул
    if ( cli!=nullptr )
      cli->stop();
  }
}

client_tcp_map::client_ptr client_tcp_map::create()
{
  std::lock_guard<mutex_type> lk(_mutex);
  return this->create_();
}
void client_tcp_map::perform_io( data_ptr d, io_id_t id, output_handler_t handler)
{
  if ( _stop_flag ) return;
  
  // Для клиента reg_io обязателен 
  if ( auto cli1 = this->find(id ) )
  {
    cli1->perform_io( std::move(d), id, std::move(handler) );
  }
  else if ( client_ptr cli2 = this->create() )
  {
    // Опционально
    cli2->perform_io( std::move(d), 0, [this, cli2, handler](data_ptr d2) mutable
    {
      this->free(cli2);
      //cli2 = nullptr;
      if (handler!=nullptr)
        handler(std::move(d2));
    });
  }
  else if ( handler != nullptr )
  {
    // Если напрямую подключили server-tcp в режиме direct_mode или server-udp
    SYSTEM_LOG_FATAL("client_tcp_map::perform_io: object (io_id=" << id << ") not registered. Probably a configuration error.")
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

client_tcp_map::client_ptr client_tcp_map::create_()
{
  client_ptr cli;
  
  if ( !_startup_pool.empty() )
  {
    cli = _startup_pool.front();
    _startup_pool.pop_front();
  }
  else if ( !_secondary_pool.empty() )
  {
    cli = _secondary_pool.front();
    _secondary_pool.pop_front();
  }
  else if ( !_primary_pool.empty() )
  {
    cli = _primary_pool.front();
    _primary_pool.pop_front();
    client_ptr new_cli = std::make_shared<client_type>(_io);
    new_cli->start(this->_opt);
    _primary_pool.push_back(new_cli);
  }
  else
  {
    cli = std::make_shared<client_type>(_io);
    cli->start(this->_opt);
  }
  return cli;
}

void client_tcp_map::free(client_ptr cli)
{
  std::lock_guard<mutex_type> lk(_mutex);
  if ( _secondary_pool.size() < _opt.secondary_pool )
    _secondary_pool.push_back( std::move(cli) );
  // Если  не перенесен в пул
  if ( cli!=nullptr )
    cli->stop();
}

}}
