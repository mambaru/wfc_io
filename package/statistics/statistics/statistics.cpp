#include <iostream>
#include "statistics.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/server/server.hpp>


namespace wfc{ namespace io{
 
void statistics::initialize()
{
  const auto& opt = this->options();
  const auto& stat_opt = this->statistics_options();
  _target = this->get_target<iinterface>( opt.target );
  this->get_workflow()->release_timer(_timer_id);
  if ( auto stat = this->get_statistics() )
  {
    _meter = stat->create_composite_meter( stat_opt.time_name, stat_opt.read_name, stat_opt.write_name, true );
    _connections_meter = stat->create_value_meter( stat_opt.track_name );
    if ( opt.tracking_ms > 0 )
    {
      _timer_id = this->get_workflow()->create_timer(
        std::chrono::milliseconds(opt.tracking_ms),
        this->wrap([this]()->bool
        {
          if ( this->suspended() )
            return true;
          DEBUG_LOG_DEBUG("io::statistics: this->owner().tracking_size()=" << this->owner().tracking_size())
          size_t size = this->owner().tracking_size();
          if ( auto stat1 = this->get_statistics() )
          {
            this->_connections_meter.create( 
              static_cast<wrtstat::value_type>(size), 
              static_cast<wrtstat::size_type>(0) 
            );
          }
          return true;
        }, nullptr)
      );
    }
  }
}

void statistics::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  this->owner().tracking(io_id);
  if (auto t = _target.lock() )
  {
    t->reg_io(io_id, itf);
  }
  super::reg_io(io_id, itf);
}

void statistics::unreg_io(io_id_t io_id)
{
  if (auto t = _target.lock() )
  {
    t->unreg_io(io_id);
  }
  
  super::unreg_io(io_id);
}

void statistics::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
{
  if (auto t = _target.lock() )
  {
    if ( this->suspended() || d == nullptr )
    {
      t->perform_io( std::move(d), io_id, std::move(handler) );
    }
    else if ( auto stat = this->get_statistics() )
    {
      auto meter = _meter.create_shared( 
        static_cast<wrtstat::size_type>(1),           // count 
        static_cast<wrtstat::value_type>(d->size()),  // readed
        static_cast<wrtstat::value_type>(0)           // writer (later)
      );
      t->perform_io( 
        std::move(d), 
        io_id,
        [handler, meter](data_ptr d2) 
        { 
          if ( d2!=nullptr)
            meter->set_write_size( static_cast<wrtstat::value_type>(d2->size()) );
          handler( std::move(d2) ); 
        } 
      );
    }
  }
}

}}
