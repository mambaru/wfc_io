#pragma once

#include <iow/ip/tcp/client/options.hpp>
#include <wfc/core/workflow_options.hpp>

namespace wfc{
  
struct client_tcp_config
  : public ::iow::ip::tcp::client::options
{
  workflow_options workflow_opt;
};

}
