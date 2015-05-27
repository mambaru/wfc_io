#pragma once

#include "domain/server_tcp_config.hpp"
#include <wfc/json.hpp>

namespace wfc{

struct server_config_json
{
  JSON_NAME(target)
  typedef json::object<
    server_tcp_config,
    wfc::json::member_list<
        json::member<n_target, server_tcp_config, std::string, &server_tcp_config::target>
    >
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
