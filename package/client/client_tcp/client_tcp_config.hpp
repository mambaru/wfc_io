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

}}
