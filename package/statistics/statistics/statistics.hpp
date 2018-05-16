#pragma once

#include "statistics_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/statistics/meters.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{

class statistics
  : public ::wfc::domain_object<iinterface, statistics_config>
{
public:
  virtual void initialize() override;
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override;
  virtual void unreg_io(io_id_t /*io_id*/) override;
  virtual void perform_io(data_ptr /*d*/, io_id_t /*io_id*/, output_handler_t handler) override;
private:
  /*typedef std::shared_ptr<composite_meter> meter_ptr;
  typedef std::shared_ptr<value_meter> connections_meter_ptr;*/
  typedef std::mutex mutex_type;
  
  std::weak_ptr<iinterface> _target;
  composite_factory _meter;
  value_factory _connections_meter;  
  std::unordered_set<io_id_t> _connections;
  mutex_type _mutex;
  timer_id_t _timer_id = -1;
};

}}
