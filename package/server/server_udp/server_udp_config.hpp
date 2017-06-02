#pragma once

#include <wfc/iinterface.hpp>
#include <iow/ip/udp/server/options.hpp>
#include <set>

namespace wfc{ namespace io{
  
struct server_udp_config
  : public ::iow::ip::udp::server::options
{
  std::string target_name;
  /*std::set<int> cpu;*/
  std::weak_ptr<iinterface> target;
};

}}
