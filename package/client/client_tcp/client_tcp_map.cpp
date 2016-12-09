#include "client_tcp_map.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

class client_tcp_map::handler_wrapper: public iinterface
{
public:
  handler_wrapper(outgoing_handler_t handler): _handler(handler) {}
  virtual void perform_io( iinterface::data_ptr d, io_id_t /*id*/, outgoing_handler_t /*handler*/) override
  {
    _handler( std::move(d) );
  }
private:
  outgoing_handler_t _handler;
};

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
    auto cli = item.second.first;
    auto holder = cli->get_holder();
    cli->stop();
    item.second.first = std::make_shared<client_type>(_io, holder);
    cli->start(opt);
  }
}

void client_tcp_map::stop()
{
  for ( auto& cli : _clients )
  {
    cli.second.first->stop();
    cli.second.first = nullptr;
  }
  _clients.clear();
}


client_tcp_map::client_ptr client_tcp_map::find( io_id_t id ) const
{
  std::lock_guard<mutex_type> lk(_mutex);
  return this->find_(id);
}

client_tcp_map::client_ptr client_tcp_map::queryset( io_id_t id, outgoing_handler_t handler)
{
  std::lock_guard<mutex_type> lk(_mutex);
  client_ptr cli = this->find_(id);
  if ( cli == nullptr )
  {
    auto hdr = std::make_shared<handler_wrapper>(handler);
    cli = std::make_shared<client_type>(_io, hdr);
    _clients.insert( std::make_pair(id, std::make_pair(cli, hdr) ) );
    cli->start(_opt);
  }
  return cli;
}

// iinterface

void client_tcp_map::reg_io( io_id_t id, std::weak_ptr<iinterface> holder)
{
  std::lock_guard<mutex_type> lk(_mutex);
  client_ptr cli = this->find_(id);
  if ( cli == nullptr )
  {
    cli = std::make_shared<client_type>(_io, holder);
    _clients.insert( std::make_pair(id, std::make_pair(cli, nullptr)) );
    cli->start(_opt);
  }
  else
  {
    //cli->set_holder(holder);
  }
}

void client_tcp_map::unreg_io( io_id_t id)
{
  std::lock_guard<mutex_type> lk(_mutex);
  auto itr = _clients.find(id);
  if ( itr == _clients.end() )
    return;
  auto cli = itr->second.first;
  _clients.erase(itr);
}

void client_tcp_map::perform_io( iinterface::data_ptr d, io_id_t id, outgoing_handler_t handler)
{
  handler = [handler]( iinterface::data_ptr d )
  {
    handler( std::move(d) );
  };
  
  DEBUG_LOG_DEBUG("-2- client_tcp::perform_io: " << d)
  if ( auto cli = this->queryset(id, handler ) )
  {
    DEBUG_LOG_DEBUG("-2.1- client_tcp::perform_io: " << d)
    cli->perform_io( std::move(d), id, std::move(handler) );
  }
  else if ( handler != nullptr )
  {
    DEBUG_LOG_DEBUG("-2.2- client_tcp::perform_io: " << d)
    handler(nullptr);
  }
}

// private

client_tcp_map::client_ptr client_tcp_map::find_( io_id_t id ) const
{
  auto itr = _clients.find(id);
  if ( itr!=_clients.end() )
    return itr->second.first;
  return nullptr;
}

}}
