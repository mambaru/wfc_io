#pragma once

#include "server_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{

class server_tcp
  : public ::wfc::domain_object<iinterface, server_tcp_config>
{
  class impl;
public:
  virtual ~server_tcp();
  server_tcp();
  virtual void start() override;
  virtual void initialize() override;
  virtual void stop() override;
private:
  std::shared_ptr<impl> _impl;
  std::weak_ptr<iinterface> _target;
};

}}
