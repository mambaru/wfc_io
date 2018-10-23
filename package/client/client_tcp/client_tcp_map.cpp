#include "client_tcp_map.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

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
    for ( auto& cli : _client_pool )
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
    _opt.connection.shutdown_handler = [this, shutdown_handler](io_id_t id)
    {
      DEBUG_LOG_BEGIN("client_tcp_map::reconfigure: shutdown_handler " << id)
      if ( shutdown_handler!=nullptr )
        shutdown_handler(id);
      DEBUG_LOG_END("client_tcp_map::reconfigure: shutdown_handler " << std::boolalpha << (shutdown_handler!=nullptr))
    };
  
    for ( auto& item : _clients )
    {
      item.second = std::make_shared<client_type>(_io);
      client_list.push_back(item.second);
      item.second->start(opt);
    }
  
    _client_pool.clear();
    while ( _client_pool.size() < opt.client_pool )
    {
      auto cli = std::make_shared<client_type>(_io);
      _client_pool.push_back(cli);
      client_list.push_back(cli);
    }
    
    _startup_pool.clear();
    if ( _startup_flag )
    {
      auto cli = std::make_shared<client_type>(_io);
      _startup_pool.push_back(cli);
      client_list.push_back(cli);
      _startup_flag = false;
    }
  }
  
  for ( auto& cli : client_list )
    cli->start(_opt);
}

void client_tcp_map::stop()
{
  for ( auto& cli : _clients )
  {
    cli.second->stop();
    cli.second = nullptr;
  }
  std::lock_guard<mutex_type> lk(_mutex);
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
  options_type opt;
  client_ptr cli;
  client_ptr cli_pool;
  bool not_started = false;
  std::shared_ptr< ::wflow::workflow> wrkfl;
  {
    std::lock_guard<mutex_type> lk(_mutex);
    opt = _opt;
    if ( !_startup_pool.empty() )
    {
      cli = _startup_pool.front();
      _startup_pool.pop_front();
    }
    else if ( !_client_pool.empty() )
    {
      cli = _client_pool.front();
      _client_pool.pop_front();
      cli_pool = std::make_shared<client_type>(_io);
      wrkfl = _opt.args.workflow;
    }
    else
    {
      not_started = true;
      cli = std::make_shared<client_type>(_io);
    }
    _clients.insert( std::make_pair(id, cli ) );
  }
  
  if ( not_started )
    cli->start(opt);
  
  if ( wrkfl!=nullptr )
  {
    wrkfl->safe_post([opt, cli_pool](){
      cli_pool->start(opt);
    });
  }
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
