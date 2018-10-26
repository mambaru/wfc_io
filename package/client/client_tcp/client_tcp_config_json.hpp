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
  JSON_NAME(rn)
  JSON_NAME(abort_if_error)
  JSON_NAME(startup_pool)
  JSON_NAME(primary_pool)
  JSON_NAME(secondary_pool)

  typedef json::object<
    client_tcp_config,
    json::member_list<
      json::member<n_startup_pool, client_tcp_config, size_t, &client_tcp_config::startup_pool>,
      json::member<n_primary_pool, client_tcp_config, size_t, &client_tcp_config::primary_pool>,
      json::member<n_secondary_pool, client_tcp_config, size_t, &client_tcp_config::secondary_pool>,
      json::member<n_abort_if_error, client_tcp_config, bool, &client_tcp_config::abort_if_error>,
      json::member<n_rn, client_tcp_config, bool, &client_tcp_config::rn>,
      json::base< ::iow::ip::tcp::client::options_json >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
