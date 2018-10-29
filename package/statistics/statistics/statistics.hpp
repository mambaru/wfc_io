//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "statistics_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/statistics/meters.hpp>
#include <unordered_map>
#include <string>
#include <memory>



namespace wfc{ namespace io{

class statistics
  : public ::wfc::domain_object<iinterface, statistics_config, defstat>
{
public:
  virtual void initialize() override;
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override;
  virtual void unreg_io(io_id_t /*io_id*/) override;
  virtual void perform_io(data_ptr /*d*/, io_id_t /*io_id*/, output_handler_t handler) override;
private:
  typedef std::mutex mutex_type;
  
  std::weak_ptr<iinterface> _target;
  composite_meter _meter;
  value_meter _connections_meter;  
  std::unordered_map<io_id_t, std::shared_ptr<iinterface> > _connections;
  mutex_type _mutex;
  timer_id_t _timer_id = -1;
};

}}
