//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "client_tcp_config.hpp"
#include <iow/ip/tcp/client/options_json.hpp>
#include <wfc/workflow.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace io{
  
struct client_tcp_config_json
{
  typedef json::object<
    client_tcp_config,
    ::wfc::json::member_list<
      ::wfc::json::base< ::iow::ip::tcp::client::options_json >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
