#include <iostream>
#include "statistics.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/server/server.hpp>


namespace wfc{ namespace io{

void statistics::initialize()
{
  auto opt = this->options();
  _target = this->get_target<iinterface>( opt.target );
  if ( auto stat = this->get_statistics() )
    _meter = stat->create_composite_prototype( opt.time_name, opt.read_name, opt.write_name );
  
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
    t->reg_io(io_id, itf);
  }
}
void statistics::unreg_io(io_id_t io_id)
{
  if (auto t = _target.lock() )
  {
    t->unreg_io(io_id);
  }
}

void statistics::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler)
{
  if (auto t = _target.lock() )
  {
    if ( this->suspended() || d == nullptr )
    {
      t->perform_io( std::move(d), io_id, std::move(handler) );
    }
    else if ( auto stat = this->get_statistics() )
    {
      auto meter = stat->create_meter( _meter, d->size() );
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
