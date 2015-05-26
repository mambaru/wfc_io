#pragma once
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include "server_tcp_config.hpp"

namespace wfc{

class server_tcp
  : public ::wfc::domain_object<iinterface, server_tcp_config>
{
public:
  virtual ~server_tcp();
  server_tcp();
};

}
