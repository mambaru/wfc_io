#pragma once

#include <iow/io/aux/data_pool_options.hpp>
#include "tcp_options.hpp"
#include <set>

namespace wfc{ namespace io{
  
struct data_pool_options: 
  ::iow::io::data_pool_options 
{
  bool disabled = false;
};

struct server_tcp_config: tcp_options
{
  std::string target;
  bool keep_alive = true;
  data_pool_options data_pool;
  /*std::set<int> cpu;*/
};

}}
