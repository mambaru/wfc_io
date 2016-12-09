#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace io{

class queue_multiton
  : public ::wfc::component
{
public:
  queue_multiton();
};

}}
