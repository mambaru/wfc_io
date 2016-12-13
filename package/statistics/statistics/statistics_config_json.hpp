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
  /*
  JSON_NAME(total_time_name)
  JSON_NAME(total_size_name)
  JSON_NAME(input_time_name)
  JSON_NAME(input_size_name)
  JSON_NAME(output_time_name)
  JSON_NAME(output_size_name)
  */
  typedef json::object<
    statistics_config,
    json::member_list<
        json::member<n_target, statistics_config, std::string, &statistics_config::target>,
        json::member<n_time_name, statistics_config, std::string, &statistics_config::time_name>,
        json::member<n_read_name, statistics_config, std::string, &statistics_config::read_name>,
        json::member<n_write_name, statistics_config, std::string, &statistics_config::write_name>
        /*
        json::member<n_target, statistics_config, std::string, &statistics_config::target>,
        json::member<n_total_time_name, statistics_config, std::string, &statistics_config::total_time_name>,
        json::member<n_total_size_name, statistics_config, std::string, &statistics_config::total_size_name>,
        json::member<n_input_time_name, statistics_config, std::string, &statistics_config::input_time_name>,
        json::member<n_input_size_name, statistics_config, std::string, &statistics_config::input_size_name>,
        json::member<n_output_time_name, statistics_config, std::string, &statistics_config::output_time_name>,
        json::member<n_output_size_name, statistics_config, std::string, &statistics_config::output_size_name>
        */
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
