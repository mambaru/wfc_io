#include "queue.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

void queue::initialize()
{
  const auto opt = this->options();
  if ( opt.callback_queue == true )
    _callback_workflow = this->get_workflow( opt.callback_workflow );
  else
    _callback_workflow = nullptr;

  _target = this->get_target<iinterface>( opt.target ) ;
}

void queue::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  if (auto t = _target.lock() )
  {
    t->reg_io(io_id, itf);
  }
}
void queue::unreg_io(io_id_t io_id)
{
  if (auto t = _target.lock() )
  {
    t->unreg_io(io_id);
  }
}


void queue::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) 
{
  auto target = _target.lock();
  
  if ( target == nullptr)
    return handler( std::move(d) );
  
  if ( this->suspended() )
    return target->perform_io( std::move( d ), io_id, std::move(handler) );

  auto pd = std::make_shared<data_ptr>( std::move(d) );
  std::weak_ptr<iinterface> wtarget = _target;
  std::weak_ptr<queue> wthis = this->shared_from_this();;
  this->get_workflow()->post(
    [pd, io_id, handler, wthis, wtarget]() mutable
    {
      if ( auto pthis = wthis.lock() )
      {
        if ( auto t = wtarget.lock() )
        {
          t->perform_io( 
              std::move( *pd ), 
              io_id, 
              pthis->make_handler_( std::move(handler) ) 
          );
        }
      }
    },
    [handler]()
    {
      handler(nullptr);
    }
  );
}

iinterface::output_handler_t queue::make_handler_(output_handler_t&& handler)
{
  if ( _callback_workflow == nullptr ) 
    return std::move(handler);
  
  std::weak_ptr<queue> wthis = this->shared_from_this();;
  return [handler, wthis](data_ptr d)
  {
    workflow_ptr w;
    if ( auto pthis = wthis.lock() )
      w = pthis->_callback_workflow;

    if ( w!=nullptr )
    {
      auto pd = std::make_shared<data_ptr>( std::move(d) );
      w->post(
        [pd, handler]() { handler( std::move( *pd ) ); }, 
        [handler]()     { handler(nullptr); } 
      );
    }
    else
    {
      handler( std::move( d ) );
    }
  };
}

}}
