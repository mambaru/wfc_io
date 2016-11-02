#pragma once

#include "io_statistics_config.hpp"
#include <wfc/json.hpp>

namespace wfc{


struct io_statistics_config_json
{
  JSON_NAME(target)
  JSON_NAME(statistics_target)

  typedef json::object<
    io_statistics_config,
    json::member_list<
        json::member<n_target, io_statistics_config, std::string, &io_statistics_config::target>,
        json::member<n_statistics_target, io_statistics_config, std::string, &io_statistics_config::statistics_target>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
