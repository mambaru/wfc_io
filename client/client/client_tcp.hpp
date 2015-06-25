#pragma once

#include "client_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <string>
#include <memory>

namespace wfc{

class client_tcp_impl;

class client_tcp
  : public ::wfc::domain_object<iinterface, client_tcp_config>
{
public:
  virtual ~client_tcp();
  client_tcp();
  virtual void start(const std::string&) override;
  virtual void reconfigure() override;
private:
  std::shared_ptr<client_tcp_impl> _impl;
  std::weak_ptr<iinterface> _target;
};

}
