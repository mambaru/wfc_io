#pragma once

//#include <iow/ip/tcp/server/options.hpp>
#include "tcp_options.hpp"

namespace wfc{ namespace io{
  
struct server_tcp_config: tcp_options
{
  std::string target;
  bool keep_alive = false;
};

}}
