#pragma once

#include <wfc/module/object.hpp>

namespace wfc{

class server_object
  : public ::wfc::object
{
  class impl;
public:
  server_object();
};

}
