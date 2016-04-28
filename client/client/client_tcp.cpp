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
    if (src.lock()==nullptr) abort();
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
      if ( opt.connection.incoming_handler == nullptr )
      {
        opt.connection.incoming_handler = [pthis](data_ptr d, io_id_t, outgoing_handler_t handler)
        {
          if ( auto src = pthis->_src.lock() )
          {
            src->perform_io(std::move(d), pthis->_id, std::move(handler) );
          }
        };
      }
      else
      {
      }

      auto connect_handler = opt.connect_handler;
      opt.connect_handler = [pthis, connect_handler]()
      {
        if ( auto src = pthis->_src.lock() )
        {
          src->reg_io( pthis->_id, pthis );
        }
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
    else
    {
      IOW_LOG_WARNING("client_tcp::client::start src not set")
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
    if ( auto rd = _client->send( std::move(d) ) )
    {
      DEBUG_LOG_ERROR("tcp_client send FAIL: " << d)
      if (handler!=nullptr)
        handler( nullptr );
    }
  }
  
  iinterface_ptr get_src() const
  {
    return _src;
  }
  
  void set_src(iinterface_ptr src)
  {
    if (src.lock()==nullptr) abort();
    _src = src;
  }

private:
  io_id_t _id;
  iinterface_ptr _src;
  client_ptr _client;
};

}


/*class client_handler: public iinterface
{
public:
  outgoing_handler_t handler)
  
};
*/

class client_tcp::impl
{
  
public:
  
  class client_handler: public iinterface
  {
  public:
    client_handler(outgoing_handler_t handler): _handler(handler) {}
    virtual void perform_io( iinterface::data_ptr d, io_id_t /*id*/, outgoing_handler_t /*handler*/) override
    {
      _handler( std::move(d) );
    }
  private:
    outgoing_handler_t _handler;
  };
  
  typedef std::mutex mutex_type;
  typedef client::io_id_t io_id_t;
  typedef std::shared_ptr<client> client_ptr;
  typedef client::io_service_type io_service_type;
  typedef std::weak_ptr< ::wfc::iinterface > iinterface_ptr;
  typedef std::pair<client_ptr, std::shared_ptr<::wfc::iinterface> > client_pair;
  typedef std::map< io_id_t, client_pair> client_map_t;
  typedef ::iow::ip::tcp::client::options options_type;
  
  
  impl( io_service_type& io)
    : _io(io)
  {
  }
  
  void reconfigure(const options_type& opt)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _opt = opt;
    for ( auto& item : _clients )
    {
      auto cli = item.second.first;
      auto src = cli->get_src();
      cli->stop();
      item.second.first = std::make_shared<client>(_io, src);
      cli->start(opt);
    }
  }
  
  void stop()
  {
    for ( auto& cli : _clients )
    {
      cli.second.first->stop();
      cli.second.first = nullptr;
    }
    _clients.clear();
  }
  
  void perform_io( iinterface::data_ptr d, io_id_t id, outgoing_handler_t handler)
  {
    if ( auto cli = this->queryset(id, handler ) )
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

  client_ptr queryset( io_id_t id, outgoing_handler_t handler)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    client_ptr cli = this->find_(id);
    if ( cli == nullptr )
    {
      auto hdr = std::make_shared<client_handler>(handler);
      cli = std::make_shared<client>(_io, hdr);
      _clients.insert( std::make_pair(id, std::make_pair(cli, hdr) ) );
      cli->start(_opt);
    }
    return cli;
  }

  client_ptr upsert( io_id_t id, iinterface_ptr src)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    client_ptr cli = this->find_(id);
    if ( cli == nullptr )
    {
      cli = std::make_shared<client>(_io, src);
      _clients.insert( std::make_pair(id, std::make_pair(cli, nullptr)) );
      cli->start(_opt);
    }
    else
    {
      cli->set_src(src);
    }
    return cli;
  }
  
  client_ptr erase( io_id_t id)
  {
    std::lock_guard<mutex_type> lk(_mutex);
    auto itr = _clients.find(id);
    if ( itr == _clients.end() )
      return nullptr;
    auto cli = itr->second.first;
    _clients.erase(itr);
    return cli;
  }

private:
  
  client_ptr find_( io_id_t id ) const
  {
    auto itr = _clients.find(id);
    if ( itr!=_clients.end() )
      return itr->second.first;
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
  DEBUG_LOG_TRACE("client_tcp::reg_io id=" << io_id)
  this->_impl->upsert( io_id, itf);
}

void client_tcp::unreg_io(io_id_t io_id)
{
  // Обязательно удаляем обработчик при закрыии источника
  DEBUG_LOG_TRACE("client_tcp::unreg_io id=" << io_id)
  _impl->erase(io_id);
}

void client_tcp::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
{
  _impl->perform_io( std::move(d), io_id, std::move(handler) );
}

}
