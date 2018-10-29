#include <iostream>
#include "statistics.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/server/server.hpp>


namespace wfc{ namespace io{
namespace {
  class composite_wrapper: public iinterface
  {
  public:
    composite_wrapper(std::weak_ptr<iinterface> itf, composite_meter meter)
      : _itf(itf)
      , _meter(meter)
    {
    }
    
    void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
    {
      if (auto t = _itf.lock() )
      {
        t->reg_io(io_id, itf);
      }
    }
    
    void unreg_io(io_id_t io_id)
    {
      if (auto t = _itf.lock() )
      {
        t->unreg_io(io_id);
      }
    }
    
    void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
    {
      if (auto t = _itf.lock() )
      {
        // TODO: было _meter.create_shared( 1, d->size(), 0 );
        // сейчас в count рамер в байтах 
        
        auto point = _meter.create_shared( 1, d->size(), 0 );
        t->perform_io( 
          std::move(d), 
          io_id,
          [handler, point](data_ptr d) 
          { 
            if ( d!=nullptr)
              point->set_write_size( d->size() );
            handler( std::move(d) ); 
          } 
        );
      }
    }
  private:  
    std::weak_ptr<iinterface> _itf;
    composite_meter _meter;
  };
}

  
void statistics::initialize()
{
  auto opt = this->options();
  _target = this->get_target<iinterface>( opt.target );
  this->get_workflow()->release_timer(_timer_id);
  if ( auto stat = this->get_statistics() )
  {
    _meter = stat->create_composite_meter( opt.time_name, opt.read_name, opt.write_name, true );
    _connections_meter = stat->create_value_meter( opt.io_name );
    if ( opt.interval_ms > 0 )
    {
      _timer_id = this->get_workflow()->create_timer(
        std::chrono::milliseconds(opt.interval_ms),
        this->wrap([this]()->bool
        {
          if ( this->suspended() )
            return true;
          size_t size = 0;
          {
            std::lock_guard<mutex_type> lk(this->_mutex);
            size = this->_connections.size();
          }
          if ( auto stat = this->get_statistics() )
            this->_connections_meter.create( size, 0);
          return true;
        }, nullptr)
      );
    }
  }
  
  /*
  _total_meter = this->create_meter_prototype( opt.total_time_name, opt.total_size_name );
  _input_meter = this->create_meter_prototype( opt.input_time_name, opt.input_size_name );
  _output_meter = this->create_meter_prototype( opt.output_time_name, opt.output_size_name );
  */
}

void statistics::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  if (auto t = _target.lock() )
  {
    if ( !this->suspended() )
    {
      if ( auto stat = this->get_statistics() )
      {
        const auto& opt = this->options();
        auto meter = stat->create_composite_meter( opt.time_name, opt.read_name, opt.write_name, true );
        std::lock_guard<mutex_type> lk(_mutex);
        auto wrpitf = std::make_shared<composite_wrapper>(itf, meter);
        _connections[io_id] = wrpitf;
        t->reg_io(io_id, wrpitf);
        return;
      }
    }
    t->reg_io(io_id, itf);
  }
}

void statistics::unreg_io(io_id_t io_id)
{
  if (auto t = _target.lock() )
  {
    t->unreg_io(io_id);
    if ( !this->suspended()  )
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _connections.erase(io_id);
    }
  }
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
      auto meter = _meter.create_shared( 1, d->size(), 0 );
      t->perform_io( 
        std::move(d), 
        io_id,
        [handler, meter](data_ptr d) 
        { 
          if ( d!=nullptr)
            meter->set_write_size( d->size() );
          handler( std::move(d) ); 
        } 
      );
    }
  }
}

}}
