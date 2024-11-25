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
  JSON_NAME(keep_alive)

  typedef json::object<
    client_tcp_config,
    json::member_list<
      json::member<n_startup_pool, client_tcp_config, size_t, &client_tcp_config::startup_pool>,
      json::member<n_primary_pool, client_tcp_config, size_t, &client_tcp_config::primary_pool>,
      json::member<n_secondary_pool, client_tcp_config, size_t, &client_tcp_config::secondary_pool>,
      json::member<n_abort_if_error, client_tcp_config, bool, &client_tcp_config::abort_if_error>,
      json::member<n_rn, client_tcp_config, bool, &client_tcp_config::rn>,
      json::member<n_keep_alive, client_tcp_config, bool, &client_tcp_config::keep_alive>,
      json::base< ::iow::ip::tcp::client::options_json >
    >,
    json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct client_tcp_stat_json
{
  JSON_NAME(tracking_ms)
  JSON_NAME(connections)
  JSON_NAME(rtotal_size)
  JSON_NAME(rtotal_capacity)
  JSON_NAME(rchunk_count)
  JSON_NAME(rchunk_count_capacity)
  JSON_NAME(wtotal_size)
  JSON_NAME(wtotal_capacity)
  JSON_NAME(wchunk_count)
  JSON_NAME(wchunk_count_capacity)

  typedef json::object<
    client_tcp_stat,
    json::member_list<
      json::member<n_tracking_ms, client_tcp_stat, time_t, &client_tcp_stat::tracking_ms, json::time_interval_ms<> >,
      json::member<n_connections, client_tcp_stat, std::string, &client_tcp_stat::connections>,
      json::member<n_rtotal_size, client_tcp_stat, std::string, &client_tcp_stat::rtotal_size>,
      json::member<n_rtotal_capacity, client_tcp_stat, std::string, &client_tcp_stat::rtotal_capacity>,
      json::member<n_rchunk_count, client_tcp_stat, std::string, &client_tcp_stat::rchunk_count>,
      json::member<n_rchunk_count_capacity, client_tcp_stat, std::string, &client_tcp_stat::rchunk_count_capacity>,
      json::member<n_wtotal_size, client_tcp_stat, std::string, &client_tcp_stat::wtotal_size>,
      json::member<n_wtotal_capacity, client_tcp_stat, std::string, &client_tcp_stat::wtotal_capacity>,
      json::member<n_wchunk_count, client_tcp_stat, std::string, &client_tcp_stat::wchunk_count>,
      json::member<n_wchunk_count_capacity, client_tcp_stat, std::string, &client_tcp_stat::wchunk_count_capacity>
    >,
    json::strict_mode
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
