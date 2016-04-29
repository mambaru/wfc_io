#pragma once

//#include <iow/ip/tcp/server/options.hpp>
#include "tcp_options.hpp"

namespace wfc{
  
struct server_tcp_config: tcp_options
{
  std::string target;
};

}
