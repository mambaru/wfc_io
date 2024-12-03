//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iow/ip/tcp/client/options.hpp>
#include <wfc/workflow.hpp>

namespace wfc{ namespace io{


struct client_tcp_config
  : public ::iow::ip::tcp::client::options
{
  bool rn = true;
  bool keep_alive = true;
  bool abort_if_error = true;
  size_t startup_pool = 1;
  size_t primary_pool = 0;
  size_t secondary_pool = 0;
};

struct client_tcp_stat
{
  time_t tracking_ms = 1000;

  std::string connections = ".connections";

  std::string rtotal_size = ".buffer_size:read";
  std::string rtotal_capacity = ".buffer_capacity:read";
  std::string rchunk_count = ".chunk_count:read";
  std::string rchunk_count_capacity = ".chunk_capacity:read";

  std::string wtotal_size = ".buffer_size:write";
  std::string wtotal_capacity = ".buffer_capacity:write";
  std::string wchunk_count = ".chunk_count:write";
  std::string wchunk_count_capacity = ".chunk_capacity:write";
};

}}
