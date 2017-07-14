#pragma once

#include "tcp_options.hpp"
#include <set>

namespace wfc{ namespace io{

struct server_tcp_config: tcp_options
{
  std::string target;
  bool keep_alive = true;
};

}}
