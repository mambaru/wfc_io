//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/iinterface.hpp>
#include <iow/ip/udp/server/options.hpp>
#include <set>

namespace wfc{ namespace io{
  
struct server_udp_config
  : public ::iow::ip::udp::server::options
{
  std::string target_name;
  std::weak_ptr<iinterface> target;
};

}}
