#pragma once

#include <iow/ip/tcp/server/options.hpp>

namespace wfc{
  
struct server_tcp_config
  : public ::iow::ip::tcp::server::options
{
  std::string target;
};

}
