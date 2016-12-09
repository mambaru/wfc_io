#include "queue.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

void queue::ready()
{
  const auto opt = this->options();
  if ( opt.callback_queue == true )
    _callback_workflow = this->get_workflow( opt.callback_workflow );
  else
    _callback_workflow = nullptr;

  _target = this->get_target<iinterface>( opt.target ) ;
}

void queue::perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) 
{
  _target->perform_io( std::move( d ), io_id, std::move(handler) );
  return;
  if ( _target == nullptr)
    return handler( std::move(d) );
  
  if ( this->suspended() )
    return _target->perform_io( std::move( d ), io_id, std::move(handler) );

  auto pd = std::make_shared<data_ptr>( std::move(d) );
  std::weak_ptr<iinterface> wtarget = _target;
  std::weak_ptr<queue> wthis = this->shared_from_this();;
  this->get_workflow()->post([pd, io_id, handler, wthis, wtarget]() mutable
  {
    if ( auto pthis = wthis.lock() )
      if ( auto t = pthis->_target )
        t->perform_io( std::move( *pd ), io_id, pthis->make_handler_( std::move(handler) ) );
  });
}

iinterface::outgoing_handler_t queue::make_handler_(outgoing_handler_t&& handler)
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
      w->post([pd, handler]()
      {
        handler( std::move( *pd ) );
      });
    }
    else
      handler( std::move( d ) );
  };
}

}}
