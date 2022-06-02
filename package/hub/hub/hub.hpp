#pragma once

#include "hub_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/mutex.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{

class hub
  : public domain_object<iinterface, hub_config, nostat>
  , public std::enable_shared_from_this<hub>
{
  typedef domain_object<iinterface, hub_config, nostat> super;
public:
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override final;
  virtual void unreg_io(io_id_t io_id) override final;
  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override final;
  
  virtual void initialize() override final;
  virtual void restart() override final;
private:
  typedef std::mutex mutex_type;
  typedef std::weak_ptr<iinterface> witf;
  mutex_type _mutex;
  std::unordered_map<io_id_t, witf> _map;
  witf _wtarget;
  std::shared_ptr<iinterface> _target_handler;
  io_id_t _handler_id = 0;
};

}}
