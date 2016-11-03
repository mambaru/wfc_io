#include <iostream>
#include "io_statistics.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/server/server.hpp>


namespace wfc{

void io_statistics::initialize()
{
  _target = this->get_target<iinterface>( this->options().target );
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
      auto start = std::chrono::high_resolution_clock::now();
      t->perform_io( std::move(d), io_id, [handler](data_ptr d) { handler( std::move(d) ); } );
      auto finish = std::chrono::high_resolution_clock::now();
      auto span = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
      std::cout << "Статистика: " << span << " наносекунд! скорость: " << 1000000000/span << std::endl;
    }
  }
}

}
