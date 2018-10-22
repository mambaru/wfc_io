//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "server_tcp_config.hpp"
#include <iow/ip/tcp/server/options_json.hpp>
#include <wfc/json.hpp>

namespace wfc{ namespace io{

struct connection_options_json
{
  JSON_NAME(direct_mode)
  
  typedef json::object<
    tcp_connection_options,
    json::member_list<
      json::base< ::iow::ip::tcp::connection::options_json >,
      json::member<n_direct_mode, tcp_connection_options, bool, &tcp_connection_options::direct_mode>
    >,
    json::strict_mode
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
  JSON_NAME(rn)
  JSON_NAME(keep_alive)

  typedef json::object<
    server_tcp_config,
    json::member_list<
        json::member<n_target, server_tcp_config, std::string, &server_tcp_config::target>,
        json::member<n_rn, server_tcp_config, bool, &server_tcp_config::rn>,
        json::member<n_keep_alive, server_tcp_config, bool, &server_tcp_config::keep_alive>,
        json::base< server_json >
    >,
    json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
