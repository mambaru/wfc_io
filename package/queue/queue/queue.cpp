#include "queue.hpp"
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

queue::queue()
{
}

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
  if ( _target = nullptr)
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
  return std::move([handler, wthis](data_ptr d)
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
  });
}

/*
void queue::stop(const std::string&)
{
  _callback_workflow = nullptr;
}

void queue::perform_incoming(incoming_holder holder, io_id_t io_id, rpc_outgoing_handler_t handler) 
{
  if ( this->suspended()  )
  {
    this->get_target().perform_incoming( std::move( holder ), io_id, std::move(handler) );
  }
  else
  {
    auto pholder = std::make_shared<incoming_holder>( std::move(holder) );
    this->get_workflow()->post([pholder, io_id, handler, this]() mutable
    {
      auto t = this->get_target();
      t.perform_incoming( std::move( *pholder ), io_id, this->make_handler_( std::move(handler) ) );
    });
  }
}
  
void queue::perform_outgoing(outgoing_holder holder, io_id_t io_id)
{
  if ( this->suspended() )
  {
    this->get_target().perform_outgoing( std::move( holder ), io_id);
  }
  else 
  {
    auto pholder = std::make_shared<outgoing_holder>( std::move(holder) );
    this->get_workflow()->post([pholder, io_id, this]()
    {
      auto t = this->get_target();
      t.perform_outgoing( std::move( *pholder ), io_id );
    });
  }
}

queue::rpc_outgoing_handler_t queue::make_handler_(rpc_outgoing_handler_t&& handler)
{
  if ( _callback_workflow == nullptr ) return std::move(handler);
  
  auto fun = [handler, this](outgoing_holder holder)
  {
    if ( auto w = this->_callback_workflow )
    {
      auto pholder = std::make_shared<outgoing_holder>( std::move(holder) );
      w->post([pholder, handler]()
      {
        handler( std::move( *pholder ) );
      });
    }
    else
    {
      // Если перекофигурировали 
      handler( std::move( holder ) );
    }
  };
  return std::move(fun);
}
*/

}}
