#include "server_udp.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/udp/server/server.hpp>


namespace wfc{ namespace io{

typedef iinterface::io_id_t io_id_t;
typedef iinterface::output_handler_t output_handler_t;
typedef iinterface::data_ptr data_ptr;

class server_udp::impl
  : public ::iow::ip::udp::server::server_base<>
{
public:
  typedef server::io_service_type io_service_type;
  impl(io_service_type& io)
    : server(io)
  {
  }
};

server_udp::~server_udp()
{
}

server_udp::server_udp()
{
}

void server_udp::initialize()
{
  if ( auto g = this->global() )
  {
    auto target = this->options().target_name;
    _target = std::make_shared<winterface>( this->get_target<iinterface>(target), this->suspended() );
  }
}

void server_udp::start()
{
  this->run_();
}

void server_udp::reconfigure_basic() 
{
  COMMON_LOG_WARNING("reconfigure_basic() ")
  _target->set_suspend(this->suspended());
}

void server_udp::restart()
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


void server_udp::run_()
{
  auto opt = this->options();
  _port = opt.port;
  _addr = opt.addr;
  auto ptarget = _target;

  if ( auto g = this->global() )
    _impl = std::make_shared<impl>( g->io_service );
  else
    return;
  using namespace std::placeholders;
  opt.input_handler = std::bind(&winterface::perform_io, ptarget, _1, _2, _3);
  opt.nonblocking = false;
  opt.target = ptarget;
  opt.thread_startup = std::bind( &server_udp::reg_thread, this );
  opt.thread_shutdown = std::bind( &server_udp::unreg_thread, this );

  this->stat_init_(&opt);

  try
  {
    _impl->start( opt );
  }
  catch(const std::exception& e)
  {
    DOMAIN_LOG_FATAL( "server_udp port: " << this->options().port << " error: " << e.what() )
  }
}

void server_udp::stat_init_(options_type* opt)
{
  if ( auto stat = this->get_statistics() )
  {
    std::weak_ptr<server_udp> wthis = this->shared_from_this();
    value_meter proto_time;
    value_meter proto_total;
    std::stringstream ss;
    proto_time = stat->create_value_meter( ss.str());
    std::stringstream ss1;
    proto_total = stat->create_value_meter( ss1.str());
      
    auto tcount = std::make_shared< std::atomic<int> >();
    opt->thread_statistics= 
      [wthis, proto_time,  tcount, opt, proto_total]
      (std::thread::id, size_t count, workflow_options::statistics_duration span) mutable
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

void server_udp::stop() 
{
  if ( _impl != nullptr )
  {
    _impl->stop();
    _impl=nullptr;
  }
}

}}
