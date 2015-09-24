#pragma once

#include <wfc/module/object.hpp>

namespace wfc{

class client_object
  : public ::wfc::object
{
  class impl;
public:
  client_object();
};

}
