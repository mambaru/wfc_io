#include <iostream>
#include "client_tcp.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/asio.hpp> // TODO: убрать 
#include <wfc/memory.hpp>

namespace wfc{

namespace{
  
class client
  : public ::wfc::iinterface
  , public std::enable_shared_from_this< client >
{
public:
  typedef ::iow::ip::tcp::client::client<> client_type;
  typedef std::shared_ptr<client_type> client_ptr;
  typedef std::weak_ptr< ::wfc::iinterface > iinterface_ptr;
  typedef iinterface::io_id_t io_id_t;
  typedef iinterface::data_ptr data_ptr;
  typedef iinterface::outgoing_handler_t outgoing_handler_t;
  typedef ::iow::ip::tcp::client::options options_type;
  typedef client_type::io_service_type io_service_type;
  
  client( io_service_type& io, iinterface_ptr src)
    : _id ( ::iow::io::create_id<io_id_t>() )
    , _src(src)
  {
    _client = std::make_shared<client_type>(io);
  }
  
  void stop()
  {
    _client->stop();
  }
  
  void start( options_type opt)
  {
    auto pthis = this->shared_from_this();
   
    if ( _src.lock() != nullptr )
    {
      auto connect_handler = opt.connect_handler;
      opt.connect_handler = [pthis, connect_handler]()
      {
        if ( auto src = pthis->_src.lock() )
          src->reg_io( pthis->_id, pthis );
        if ( connect_handler ) connect_handler();
      };
      
      auto error_handler = opt.error_handler;
      opt.error_handler = [pthis, error_handler]( ::iow::system::error_code ec )
      {
        if ( auto src = pthis->_src.lock() )
          src->unreg_io( pthis->_id);
        if ( error_handler ) error_handler(ec);
      };
    }
    
    _client->start(opt);
  }
  
  // iinterface
  
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override
  {
    
  }

  virtual void unreg_io(io_id_t /*io_id*/) override
  {
    
  }

  virtual void perform_io( iinterface::data_ptr d, io_id_t /*io_id*/, outgoing_handler_t handler) override
  {
    DEBUG_LOG_MESSAGE("client_tcp:client " << d)
    if ( auto rd = _client->send( std::move(d) ) )
    {
      if (handler!=nullptr)
        handler( nullptr );
    }
  }
  
  iinterface_ptr get_src() const
  {
    return _src;
  }

private:
  io_id_t _id;
  iinterface_ptr _src;
  client_ptr _client;
};

}

class client_tcp::impl
{
public:
  typedef std::mutex mutex_type;
  typedef client::io_id_t io_id_t;
  typedef std::shared_ptr<client> client_ptr;
  typedef client::io_service_type io_service_type;
  typedef std::map< io_id_t, client_ptr> client_map_t;
  typedef ::iow::ip::tcp::client::options options_type;
  typedef std::weak_ptr< ::wfc::iinterface > iinterface_ptr;
  
  impl( io_service_type& io)
    : _io(io)
  {
  }
  
  void reconfigure(const options_type& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _opt = opt;
    for ( auto& cli : _clients )
    {
      auto src = cli.second->get_src();
      cli.second->stop();
      cli.second = std::make_shared<client>(_io, src);
      cli.second->start(opt);
    }
  }
  
  void stop()
  {
    for ( auto& cli : _clients )
    {
      cli.second->stop();
      cli.second = nullptr;
    }
    _clients.clear();
  }
  
  void perform_io( iinterface::data_ptr d, io_id_t id, outgoing_handler_t handler)
  {
    if ( auto cli = this->queryset(id, iinterface_ptr() ) )
    {
      cli->perform_io( std::move(d), id, std::move(handler) );
    }
    else if ( handler != nullptr )
    {
      handler(nullptr);
    }
  }
  
  client_ptr find( io_id_t id ) const
  {
    std::lock_guard<mutex_type> lk(_mutex);
    return this->find_(id);
  }
  
  client_ptr queryset( io_id_t id, iinterface_ptr src)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    client_ptr cli = this->find_(id);
    if ( cli == nullptr )
    {
      cli = std::make_shared<client>(_io, src);
      _clients.insert( std::make_pair(id, cli) );
    }
    return cli;
  }
  
  client_ptr erase( io_id_t id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _clients.find(id);
    if ( itr == _clients.end() )
      return nullptr;
    auto cli = itr->second;
    _clients.erase(itr);
    return cli;
  }

private:
  
  client_ptr find_( io_id_t id ) const
  {
    auto itr = _clients.find(id);
    if ( itr!=_clients.end() )
      return itr->second;
    return nullptr;
  }
  
private:
  io_service_type& _io;
  options_type _opt;
  client_map_t _clients;
  mutable std::mutex _mutex;
};
  

  

  /*
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
*/

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
    _impl = std::make_shared<client_tcp::impl>( g->io_service);
    _impl->reconfigure( this->options() );
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
  this->_impl->queryset( io_id, itf);
}

void client_tcp::unreg_io(io_id_t io_id)
{
  // Обязательно удаляем обработчик при закрыии источника
  _impl->erase(io_id);
}

void client_tcp::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
{
  DEBUG_LOG_MESSAGE("client_tcp: " << d)
  _impl->perform_io( std::move(d), io_id, std::move(handler) );
}

}
