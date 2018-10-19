//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "server_udp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{

class server_udp
  : public ::wfc::domain_object<iinterface, server_udp_config>
  , public std::enable_shared_from_this<server_udp>
{
  class impl;
  typedef ::wfc::domain_object<iinterface, server_udp_config> self;
public:
  virtual ~server_udp();
  server_udp();
  virtual void start() override;
  virtual void initialize() override;
  virtual void stop() override;
private:
  std::shared_ptr<impl> _impl;
  std::weak_ptr<iinterface> _target;
};

}}
