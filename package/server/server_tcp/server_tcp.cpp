#include <iostream>
#include <exception>
#include "server_tcp.hpp"
#include "tcp_acceptor.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/server/server.hpp>


namespace wfc{ namespace io{
  
typedef iinterface::io_id_t io_id_t;
typedef iinterface::output_handler_t output_handler_t;
typedef iinterface::data_ptr data_ptr;

class target_wrapper
  : public iinterface
{
public:
  
  target_wrapper(std::weak_ptr<iinterface> target, bool suspend)
    : _target(target)
    , _suspend(suspend)
  {
  }

  virtual void unreg_io(io_id_t id) override
  {
    auto target = _target.lock();
    if ( !_suspend && target!=nullptr )
    {
      target->unreg_io(id);
    }
  }

  virtual void reg_io(io_id_t id, std::weak_ptr<iinterface> itf) override
  {
    auto target = _target.lock();
    if ( !_suspend && target!=nullptr )
    {
      target->reg_io(id, itf);
    }
  }
  
  virtual void perform_io(data_ptr d, io_id_t id, output_handler_t cb) override
  {
    auto target = _target.lock();
    if ( !_suspend && target!=nullptr )
    {
      target->perform_io(std::move(d), id, cb);
    }
    else if (_suspend)
    {
      cb(std::move(d));
    }
    else
    {
      cb(nullptr);
    }
  }
  
  void set_target(std::weak_ptr<iinterface> target)
  {
    _target = target;
  }
  
  void set_suspend( bool suspend)
  {
    _suspend = suspend;
  }
  
private:
  std::weak_ptr<iinterface> _target;
  std::atomic<bool> _suspend;
};

class server_tcp::impl
  : public ::iow::ip::tcp::server::server<tcp_acceptor>
{
public:
  typedef server::io_service_type io_service_type;
  impl(io_service_type& io)
    : server(io)
  {}
};


server_tcp::~server_tcp()
{
}

server_tcp::server_tcp()
{
}

void server_tcp::initialize()
{
  if ( auto g = this->global() )
  {
    auto target = this->options().target;
    _target = std::make_shared<target_wrapper>( this->get_target<iinterface>(target), this->suspended() );
  }

}

void server_tcp::start()
{
  this->run_();
}

void server_tcp::reconfigure_basic() 
{
  COMMON_LOG_WARNING("reconfigure_basic() ")
  _target->set_suspend(this->suspended());
}

void server_tcp::restart()
{
  auto opt = this->options();
  auto shutdown = std::move(_impl);
  if ( _port!=opt.port || _addr!=opt.addr )
  {
    this->get_workflow()->post([shutdown](){
      shutdown->stop();
    });
  }
  else
  {
    shutdown->stop();
  }
  this->run_();
}


void server_tcp::run_()
{
  auto opt = this->options();
  _port = opt.port;
  _addr = opt.addr;
  bool keep_alive = opt.keep_alive;
  auto ptarget = _target;

  if ( auto g = this->global() )
    _impl = std::make_shared<impl>( g->io_service );
  else
    return;
    
  opt.connection.input_handler = 
    [ptarget, keep_alive](data_ptr d, io_id_t id, output_handler_t cb )
  {
    //if ( auto ptarget = wtarget.lock() )
    {
      if ( !keep_alive )
      {
        cb = [cb](data_ptr d) {
          cb(std::move(d));
          cb(nullptr);
        };
      }
      ptarget->perform_io(std::move(d), id, cb);
    }
    /*else
    {
      cb( std::move(d));
    }*/
  };
  /*
    if ( opt.keep_alive ) 
    {
      opt.connection.input_handler = 
        [wtarget]( data_ptr d, io_id_t id, output_handler_t cb ) 
      {
        if ( auto ptarget = wtarget.lock() )
        {
          ptarget->perform_io(std::move(d), id, std::move(cb));
        }
        else
        {
          cb( std::move(d));
        }
      };
    }
    else
    {
      opt.connection.input_handler = 
        [wtarget](data_ptr d, io_id_t id, output_handler_t cb )
      {
        if ( auto ptarget = wtarget.lock() )
        {
          ptarget->perform_io(std::move(d), id, [cb](data_ptr d)
          {
            cb(std::move(d));
            cb(nullptr);
          });
        }
        else
        {
          cb( std::move(d));
        }
      };
    }*/

    // По дефолту nonblocking=true, но мы вешаем accept на поток, поэтому нужен блокируемый 
    opt.nonblocking = false;

    opt.connection.target = ptarget;
    opt.thread_startup = std::bind( &server_tcp::reg_thread, this );
    opt.thread_shutdown = std::bind( &server_tcp::unreg_thread, this );

    if ( auto stat = this->get_statistics() )
    {
      std::weak_ptr<server_tcp> wthis = this->shared_from_this();
      value_meter proto_time;
      value_meter proto_total;
      auto tcount = std::make_shared< std::atomic<int> >();
              size_t id = tcount->fetch_add(1);
              std::stringstream ss;
              ss << this->name() << ".thread" << id;
              proto_time = stat->create_value_meter( ss.str());
              std::stringstream ss1;
              ss1 << this->name() << ".threads";
              proto_total = stat->create_value_meter( ss1.str());
      
      opt.thread_statistics= [wthis, proto_time,  tcount, opt, proto_total](std::thread::id, size_t count, workflow_options::statistics_duration span) mutable
      {
        if ( auto pthis = wthis.lock() )
        {
          if ( auto stat = pthis->get_statistics() )
          {
            auto span_mcs = std::chrono::duration_cast<std::chrono::microseconds>(span).count();
            proto_time.create(span_mcs, count );
            proto_total.create(span_mcs, count );
          }
        }
      };
    }

  try
  {
    _impl->start( opt );
  }
  catch(const std::exception& e)
  {
    DOMAIN_LOG_FATAL( "server_tcp port: " << this->options().port << " error: " << e.what() )
  }
  
}

void server_tcp::stop() 
{
  if ( _impl != nullptr )
  {
    _impl->stop();
    _impl=nullptr;
  }
}

}}
