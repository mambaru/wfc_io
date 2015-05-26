#pragma once

#include "server_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <string>
#include <memory>

namespace wfc{

class server_tcp_impl;

class server_tcp
  : public ::wfc::domain_object<iinterface, server_tcp_config>
{
public:
  virtual ~server_tcp();
  server_tcp();
  virtual void start(const std::string&);
private:
  std::shared_ptr<server_tcp_impl> _impl;
};

}
