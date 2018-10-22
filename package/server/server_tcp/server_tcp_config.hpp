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

}}
