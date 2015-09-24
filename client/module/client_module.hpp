#pragma once

#include <wfc/module/module.hpp>

namespace wfc{



class client_module
  : public ::wfc::module
{
  class impl;
public:
  client_module();
};

}
