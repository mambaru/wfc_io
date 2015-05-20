#pragma once
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include "server_config.hpp"

namespace wfc{

class server
  : public ::wfc::domain_object<iinterface, server_config>
{
public:
  virtual ~server();
  server();
};

}
