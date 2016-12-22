#pragma once

//#include <iow/ip/tcp/server/options.hpp>
#include "tcp_options.hpp"
#include <set>

namespace wfc{ namespace io{
  
struct server_tcp_config: tcp_options
{
  std::string target;
  bool keep_alive = true;
  std::set<int> cpu;
};

}}
