#pragma once

#include "server_udp_config.hpp"
#include <iow/ip/udp/server/options_json.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace io{


struct server_udp_config_json
{
  struct server_json: ::iow::ip::udp::server::options_json{};

  JSON_NAME(target)
  JSON_NAME(cpu)

  typedef json::object<
    server_udp_config,
    json::member_list<
        json::member<n_target, server_udp_config, std::string, &server_udp_config::target_name>,
        json::base< server_json >
    >,
    json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
