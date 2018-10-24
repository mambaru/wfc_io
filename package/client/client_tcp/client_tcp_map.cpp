#include "client_tcp_map.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

class client_tcp_map::handler_wrapper: public iinterface
{
public:
  handler_wrapper(output_handler_t handler): _handler(handler) {}
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

  /*
  for ( auto& cli : _clients )
  {
    cli.second->stop();
    cli.second = nullptr;
  }
  std::lock_guard<mutex_type> lk(_mutex);
  _clients.clear();
  */
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
      DEBUG_LOG_MESSAGE("client_tcp_map: client from startup_pool")
      cli = _startup_pool.front();
      _startup_pool.pop_front();
    }
    else if ( !_secondary_pool.empty() )
    {
      DEBUG_LOG_MESSAGE("client_tcp_map: client from secondary_pool")
      cli = _secondary_pool.front();
      _secondary_pool.pop_front();
    }
    else if ( !_primary_pool.empty() )
    {
      DEBUG_LOG_MESSAGE("client_tcp_map: client from primary_pool")
      cli = _primary_pool.front();
      _primary_pool.pop_front();
      cli_pool = std::make_shared<client_type>(_io);
      wrkfl = _opt.args.workflow;
      _primary_pool.push_back(cli_pool);
    }
    else
    {
      DEBUG_LOG_MESSAGE("client_tcp_map: new client ")
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

void client_tcp_map::perform_io( data_ptr d, io_id_t id, output_handler_t handler)
{
  if ( _stop_flag ) return;
  
  // Для клиента reg_io обязателен 
  if ( auto cli = this->find(id ) )
  {
    cli->perform_io( std::move(d), id, std::move(handler) );
  }
  else if ( client_ptr cli = this->upsert(id) )
  {
    DEBUG_LOG_MESSAGE("Опционально client_tcp_map::perform_io io_id=" << id)
    // Опционально
    cli->reg_io(id, std::make_shared<iinterface>());
    cli->perform_io( std::move(d), id, [this, id, handler](data_ptr d)
    {
      this->unreg_io(id);
      if (handler!=nullptr)
        handler(std::move(d));
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

}}
