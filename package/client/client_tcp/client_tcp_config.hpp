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
};

}}
