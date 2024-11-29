//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "statistics_config.hpp"
#include <wfc/json.hpp>

namespace wfc{ namespace io{


struct statistics_config_json
{
  JSON_NAME(target)

  typedef json::object<
    statistics_config,
    json::member_list<
      json::member<n_target, statistics_config, std::string, &statistics_config::target>
    >,
    json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct statistics_stat_json
{
  JSON_NAME(tracking_ms)
  JSON_NAME(time_name)
  JSON_NAME(read_name)
  JSON_NAME(write_name)
  JSON_NAME(track_name)

  typedef json::object<
    statistics_stat,
    json::member_list<
      json::member<n_tracking_ms, statistics_stat, time_t, &statistics_stat::tracking_ms, json::time_interval_ms<> >,
      json::member<n_track_name, statistics_stat, std::string, &statistics_stat::track_name>,
      json::member<n_time_name, statistics_stat, std::string,  &statistics_stat::time_name>,
      json::member<n_read_name, statistics_stat, std::string,  &statistics_stat::read_name>,
      json::member<n_write_name, statistics_stat, std::string, &statistics_stat::write_name>
    >,
    json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
