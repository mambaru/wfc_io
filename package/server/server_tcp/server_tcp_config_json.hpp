#pragma once

#include "server_tcp_config.hpp"
#include <iow/ip/tcp/server/options_json.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace io{

struct connection_options_json
{
  typedef json::object<
    tcp_connection_options,
    json::member_list<
        json::base< ::iow::ip::tcp::connection::options_json >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
  
};

struct server_tcp_config_json
{
  struct server_json: ::iow::ip::tcp::server::options_json< 
    tcp_options, 
    ::iow::ip::tcp::acceptor::options_json< tcp_acceptor_options, connection_options_json >
  > {};
  
  JSON_NAME(target)

  typedef json::object<
    server_tcp_config,
    json::member_list<
        json::base< server_json >,
        json::member<n_target, server_tcp_config, std::string, &server_tcp_config::target>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
