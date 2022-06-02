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


class server_tcp::impl
  : public ::iow::ip::tcp::server::server<tcp_acceptor>
{
public:
  typedef server::io_context_type io_context_type;
  explicit impl(io_context_type& io)
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
    _target = std::make_shared<winterface>( this->get_target<iinterface>(target), this->suspended() );
  }
}

void server_tcp::start()
{
  this->run_();
}

void server_tcp::reconfigure_basic() 
{
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

  _impl = std::make_shared<impl>( this->get_workflow()->get_io_context() );
    
  opt.connection.input_handler = 
    [ptarget, keep_alive](data_ptr d, io_id_t id, output_handler_t cb )
  {
    if ( !keep_alive )
    {
      cb = [cb](data_ptr d1) {
        cb(std::move(d1));
        cb(nullptr);
      };
    }
    ptarget->perform_io(std::move(d), id, cb);
  };
  
  // По дефолту nonblocking=true, но мы вешаем accept на поток, поэтому нужен блокируемый 
  opt.nonblocking = false;
  opt.connection.target = ptarget;
  opt.thread_startup = std::bind( &server_tcp::reg_thread, this );
  opt.thread_shutdown = std::bind( &server_tcp::unreg_thread, this );

  if ( opt.rn )
  {
    if ( opt.connection.reader.sep.empty() ) opt.connection.reader.sep = "\r\n";
    if ( opt.connection.writer.sep.empty() ) opt.connection.writer.sep = "\r\n";
  }

  this->stat_init_(&opt);

  try
  {
    _impl->start( opt );
  }
  catch(const std::exception& e)
  {
    DOMAIN_LOG_FATAL( "server_tcp: " << this->options().port << " error: " << e.what() )
  }
}

void server_tcp::stat_init_(options_type* opt)
{
  if ( auto stat = this->get_statistics() )
  {
    std::weak_ptr<server_tcp> wthis = this->shared_from_this();
    value_meter proto_time;
    value_meter proto_total;
    auto tcount = std::make_shared< std::atomic<int> >();
      
    size_t id = size_t(tcount->fetch_add(1));
    std::stringstream ss;
    ss << this->name() << ".thread" << id;
    proto_time = stat->create_value_meter( ss.str());
    std::stringstream ss1;
    ss1 << this->name() << ".threads";
    proto_total = stat->create_value_meter( ss1.str());
      
    opt->thread_statistics= [wthis, proto_time,  tcount, proto_total](std::thread::id, size_t count, wflow::workflow_handlers::statistics_duration span) mutable
    {
      if ( auto pthis = wthis.lock() )
      {
        if ( pthis->get_statistics()!=nullptr )
        {
          auto span_mcs = std::chrono::duration_cast<std::chrono::microseconds>(span).count();
          proto_time.create(span_mcs, count );
          proto_total.create(span_mcs, count );
        }
      }
    };
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
