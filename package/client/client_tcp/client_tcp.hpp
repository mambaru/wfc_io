//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "client_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/workflow.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{

class client_tcp_map;

class client_tcp
  : public ::wfc::domain_object< iinterface, client_tcp_config, client_tcp_stat>
  , public std::enable_shared_from_this<client_tcp>
{
public:
  client_tcp();
  // domain_object  
  virtual void initialize() override;
  virtual void start() override;
  virtual void restart() override;
  virtual void stop() override;
  
  // iinterface
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override;

  virtual void unreg_io(io_id_t /*io_id*/) override;

  virtual void perform_io(data_ptr /*d*/, io_id_t /*io_id*/, output_handler_t handler) override;

private:
  void reconfigure_and_start_();
  std::shared_ptr<client_tcp_map> _client_map;
  
  typedef std::recursive_mutex mutex_type;
  mutex_type _mutex;
  std::map<io_id_t, std::weak_ptr<iinterface> > _wait_list;
  std::atomic_bool _client_started;
  timer_id_t _timer_id = -1;
};

}}
