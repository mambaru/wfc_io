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
  JSON_NAME(time_name)
  JSON_NAME(read_name)
  JSON_NAME(write_name)
  JSON_NAME(io_name)
  JSON_NAME(interval_ms)

  typedef json::object<
    statistics_config,
    json::member_list<
        json::member<n_target, statistics_config, std::string, &statistics_config::target>,
        json::member<n_interval_ms, statistics_config, time_t, &statistics_config::interval_ms>,
        json::member<n_io_name, statistics_config, std::string, &statistics_config::io_name>,
        json::member<n_time_name, statistics_config, std::string, &statistics_config::time_name>,
        json::member<n_read_name, statistics_config, std::string, &statistics_config::read_name>,
        json::member<n_write_name, statistics_config, std::string, &statistics_config::write_name>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
