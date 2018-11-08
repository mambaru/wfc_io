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
  JSON_NAME(tracking_ms)

  typedef json::object<
    statistics_config,
    json::member_list<
      json::member<n_target, statistics_config, std::string, &statistics_config::target>,
      json::member<n_tracking_ms, statistics_config, time_t, &statistics_config::tracking_ms>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct statistics_stat_json
{
  JSON_NAME(time_name)
  JSON_NAME(read_name)
  JSON_NAME(write_name)
  JSON_NAME(track_name)

  typedef json::object<
    statistics_stat,
    json::member_list<
        json::member<n_track_name, statistics_stat, std::string, &statistics_stat::track_name>,
        json::member<n_time_name, statistics_stat, std::string,  &statistics_stat::time_name>,
        json::member<n_read_name, statistics_stat, std::string,  &statistics_stat::read_name>,
        json::member<n_write_name, statistics_stat, std::string, &statistics_stat::write_name>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
