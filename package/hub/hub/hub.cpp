#include "hub.hpp"
#include <wfc/logger.hpp>


namespace wfc{ namespace io{
  
namespace{
  class target_handler
    : public iinterface
  {
  public:
    target_handler(io_id_t id, std::weak_ptr<iinterface> owner)
      : _id(id)
      , _owner(owner)
    {}
    
    virtual void perform_io(data_ptr d, io_id_t, output_handler_t cb) override
    {
      if ( auto powner = _owner.lock() )
        powner->perform_io( std::move(d), _id, cb);
    }

  private:
    io_id_t _id = 0;
    std::weak_ptr<iinterface> _owner;
  };
}

void hub::initialize() 
{
  std::lock_guard<mutex_type> lk(_mutex);
  _wtarget = super::get_target<iinterface>( super::options().target);
}

void hub::restart() 
{
  if ( auto ptarget = _wtarget.lock() )
  {
    _handler_id = iow::io::create_id<io_id_t>();
    _target_handler = std::make_shared<target_handler>(_handler_id, this->shared_from_this() );
    ptarget->reg_io(_handler_id, _target_handler );
  }  
}

void hub::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  if ( super::suspended() )
    return;
  
  std::lock_guard<mutex_type> lk(_mutex);
  _map[io_id] = itf;
}

void hub::unreg_io(io_id_t io_id)
{
  if ( super::suspended() )
    return;

  domain_object::unreg_io(io_id);
  if (auto t = _wtarget.lock() )
  {
    t->unreg_io(io_id);
  }

  std::lock_guard<mutex_type> lk(_mutex);
  _map.erase(io_id);
}


void hub::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
{
  if ( super::suspended() )
    return;

  for (auto &i : _map)
  {
    if ( i.first != io_id )
    {
      if ( auto pitf = i.second.lock() )
      {
        pitf->perform_io(std::make_unique<data_type>(*d), super::get_id(), nullptr);
      }
    }
  }

  if ( auto ptarget = _wtarget.lock() )
  {
    if ( _handler_id != io_id )
    {
      ptarget->perform_io( std::move(d), _handler_id, handler);
    }
  }
}

}}
