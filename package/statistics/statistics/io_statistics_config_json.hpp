#pragma once

#include "io_statistics_config.hpp"
#include <wfc/json.hpp>

namespace wfc{


struct io_statistics_config_json
{
  JSON_NAME(target)
  JSON_NAME(total_time_name)
  JSON_NAME(total_size_name)
  JSON_NAME(input_time_name)
  JSON_NAME(input_size_name)
  JSON_NAME(output_time_name)
  JSON_NAME(output_size_name)

  typedef json::object<
    io_statistics_config,
    json::member_list<
        json::member<n_target, io_statistics_config, std::string, &io_statistics_config::target>,
        json::member<n_total_time_name, io_statistics_config, std::string, &io_statistics_config::total_time_name>,
        json::member<n_total_size_name, io_statistics_config, std::string, &io_statistics_config::total_size_name>,
        json::member<n_input_time_name, io_statistics_config, std::string, &io_statistics_config::input_time_name>,
        json::member<n_input_size_name, io_statistics_config, std::string, &io_statistics_config::input_size_name>,
        json::member<n_output_time_name, io_statistics_config, std::string, &io_statistics_config::output_time_name>,
        json::member<n_output_size_name, io_statistics_config, std::string, &io_statistics_config::output_size_name>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
