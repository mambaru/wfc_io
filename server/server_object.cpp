
#include "server_object.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/json.hpp>
#include "domain/server_tcp.hpp"
#include "server_tcp_config_json.hpp"

namespace wfc{
  
JSON_NAME2(server_tcp_object_name, "server-tcp")

class server_object_impl: public ::wfc::multiton<
  server_tcp_object_name,
  ::wfc::instance<server_tcp>,
  server_config_json
>
{};

server_object::server_object()
  : object( std::make_shared<server_object_impl>() )
{}

}
