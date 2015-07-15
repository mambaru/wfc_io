#pragma once

#include "../client/client_tcp_config.hpp"
#include <iow/ip/tcp/client/options_json.hpp>
#include <wfc/json.hpp>

namespace wfc{
  
struct client_tcp_config_json
{
  JSON_NAME(target)

  typedef json::object<
    client_tcp_config,
    ::wfc::json::member_list<
      /*
      ::wfc::json::base< ::iow::ip::tcp::client::options_json >,
      ::wfc::json::member<n_target, client_tcp_config, std::string, &client_tcp_config::target>
      */
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
