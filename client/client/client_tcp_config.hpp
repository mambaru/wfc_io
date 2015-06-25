#pragma once

#include <iow/ip/tcp/client/options.hpp>

namespace wfc{
  
struct client_tcp_config
  : public ::iow::ip::tcp::client::options
{
  std::string target;
};

}
