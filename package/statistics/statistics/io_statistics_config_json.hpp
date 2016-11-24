#pragma once

#include "io_statistics_config.hpp"
#include <wfc/json.hpp>

namespace wfc{


struct io_statistics_config_json
{
  JSON_NAME(target)
  JSON_NAME(rate_name)
  JSON_NAME(size_name)

  typedef json::object<
    io_statistics_config,
    json::member_list<
        json::member<n_target, io_statistics_config, std::string, &io_statistics_config::target>,
        json::member<n_rate_name, io_statistics_config, std::string, &io_statistics_config::rate_name>,
        json::member<n_size_name, io_statistics_config, std::string, &io_statistics_config::size_name>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
