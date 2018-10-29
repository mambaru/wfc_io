#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace io{ 

class broker_multiton
  : public ::wfc::component
{
public:
  broker_multiton();
};

}}
