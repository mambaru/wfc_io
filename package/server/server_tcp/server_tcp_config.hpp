//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "tcp_options.hpp"
#include <set>

namespace wfc{ namespace io{

struct server_tcp_config: tcp_options
{
  std::string target;
  bool rn = true;
  bool keep_alive = true;
};

struct server_tcp_stat
{
  time_t tracking_ms = 1000;

  std::string connections = ".connections";

  std::string rbuffer_size = ".buffer_size:read";
  std::string rbuffer_capacity = ".buffer_capacity:read";
  std::string rchunk_count = ".chunk_count:read";
  std::string rchunk_capacity = ".chunk_capacity:read";

  std::string wbuffer_size = ".buffer_size:write";
  std::string wbuffer_capacity = ".buffer_capacity:write";
  std::string wchunk_count = ".chunk_count:write";
  std::string wchunk_capacity = ".chunk_capacity:write";
};

}}
