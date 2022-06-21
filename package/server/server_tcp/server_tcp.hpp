//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "server_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/winterface.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{

class server_tcp
  : public domain_object<iinterface, server_tcp_config>
  , public std::enable_shared_from_this<server_tcp>
{
  typedef domain_object<iinterface, server_tcp_config> super;
  class impl;
public:
  virtual ~server_tcp();
  server_tcp();
  virtual void reconfigure_basic() override;
  virtual void initialize() override;
  virtual void start() override;
  virtual void restart() override;
  virtual void stop() override;
private:
  void run_();
  void stat_init_(options_type* opt);
  std::shared_ptr<impl> _impl;
  std::string _port;
  std::string _addr;
  std::shared_ptr<winterface> _target;
};

}}
