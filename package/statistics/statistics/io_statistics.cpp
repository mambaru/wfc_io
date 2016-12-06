#include <iostream>
#include "io_statistics.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/server/server.hpp>


namespace wfc{

void io_statistics::initialize()
{
  auto opt = this->options();
  _target = this->get_target<iinterface>( opt.target );
  _total_meter = this->create_meter_prototype( opt.total_time_name, opt.total_size_name );
  _input_meter = this->create_meter_prototype( opt.input_time_name, opt.input_size_name );
  _output_meter = this->create_meter_prototype( opt.output_time_name, opt.output_size_name );
}

void io_statistics::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  if (auto t = _target.lock() )
  {
    t->reg_io(io_id, itf);
  }
}
void io_statistics::unreg_io(io_id_t io_id)
{
  if (auto t = _target.lock() )
  {
    t->unreg_io(io_id);
  }
}

void io_statistics::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler)
{
  if (auto t = _target.lock() )
  {
    if ( this->suspended() )
    {
      t->perform_io( std::move(d), io_id, std::move(handler) );
    }
    else
    {
      auto tmeter = this->create_meter( _total_meter, d->size(), d->size() );
      auto imeter = this->create_meter( _input_meter, d->size(), d->size() );
      t->perform_io( 
        std::move(d), 
        io_id,
        [handler, tmeter, this](data_ptr d) 
        { 
          if ( d!=nullptr)
          {
            auto ometer = this->create_meter( this->_output_meter, d->size(), d->size() );
            if ( tmeter )
              tmeter->inc( d->size(), d->size() );
          }
          handler( std::move(d) ); 
        } 
      );
      imeter.reset();
    }
  }
}

}
