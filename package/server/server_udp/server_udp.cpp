#include <iostream>
#include "server_udp.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/udp/server/server.hpp>


namespace wfc{ namespace io{

typedef iinterface::io_id_t io_id_t;
typedef iinterface::output_handler_t output_handler_t;
typedef iinterface::data_ptr data_ptr;

class server_udp::impl
  : public ::iow::ip::udp::server::server<>
{
public:
  typedef server::io_service_type io_service_type;
  impl(io_service_type& io)
    : server(io)
  {}
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
    _target = g->registry.get<iinterface>(target);
  }
}

void server_udp::start()
{
  if ( auto g = this->global() )
  {
    _impl = std::make_shared<impl>( g->io_service );
    auto opt = this->options();

    auto wtarget = _target;

    /// #warning убрать, но в базовых не должно nonblocking, т.к. nonblocking() для акцепт вылетает сразу 
    opt.nonblocking = false;

    opt.input_handler = 
        [wtarget]( data_ptr d, io_id_t id, output_handler_t callback ) 
    {
      if ( auto ptarget = wtarget.lock() )
      {
        ptarget->perform_io(std::move(d), id, std::move(callback));
      }
      else
      {
        callback( std::move(d));
      }
    };
    opt.target = wtarget;
    opt.thread_startup = std::bind( &self::reg_thread, this );
    opt.thread_shutdown = std::bind( &self::unreg_thread, this );

    /*
    std::string name = this->name();
    g->threads.set_reg_cpu(name, opt.cpu);
    opt.thread_startup = [g, name, this](std::thread::id)
    {
      g->threads.reg_thread(name);
      this->reg_thread();
    };

    opt.thread_shutdown = [g, this](std::thread::id)
    {
      g->threads.unreg_thread();
      this->unreg_thread();
    };
    */
    
    if ( auto stat = this->get_statistics() )
    {
      std::weak_ptr<server_udp> wthis = this->shared_from_this();
      typedef wfc::statistics::value_meter_ptr value_meter_ptr;
      value_meter_ptr proto_time;
      value_meter_ptr proto_total;
      auto tcount = std::make_shared< std::atomic<int> >();
      opt.thread_statistics= [wthis, proto_time,  tcount, opt, proto_total](std::thread::id, size_t count, workflow_options::statistics_duration span) mutable
      {
        if ( auto pthis = wthis.lock() )
        {
          if ( auto stat = pthis->get_statistics() )
          {
            if ( proto_time == nullptr )
            {
              //size_t id = tcount->fetch_add(1);
              std::stringstream ss;
              proto_time = stat->create_value_prototype( ss.str());
              std::stringstream ss1;
              proto_total = stat->create_value_prototype( ss1.str());
            }
            else
            {
              auto span_mcs = std::chrono::duration_cast<std::chrono::microseconds>(span).count();
              stat->create_meter(proto_time, span_mcs, count );
              stat->create_meter(proto_total, span_mcs, count );
            }
          }
        }
      };
    }
    _impl->start( opt );
  }
}

void server_udp::stop() 
{
  if ( _impl != nullptr )
  {
    CONFIG_LOG_BEGIN("Server stop")
    _impl->stop();
    CONFIG_LOG_END("Server stop")
    _impl=nullptr;
  }
}

}}
