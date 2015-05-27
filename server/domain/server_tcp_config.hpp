#pragma once

#include <iow/ip/tcp/server/options.hpp>

namespace wfc{
  
/*struct connection_config
  : public ::iow::ip::tcp::connection::options
{
  
};
*/
  
struct server_tcp_config
  : public ::iow::ip::tcp::server::options
{
  std::string target;
  //size_t backlog = 1024;
  //connection_config connection;
  //std::string host /*= "0.0.0.0"*/;
  //std::string port /*= "12345"*/;

};

}
