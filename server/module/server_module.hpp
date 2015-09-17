#pragma once

#include <wfc/module/module.hpp>

namespace wfc{

class server_module
  : public ::wfc::module
{
  class impl;
public:
  server_module();
};

}
