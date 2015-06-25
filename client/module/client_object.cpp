
#include "client_object.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/json.hpp>
#include "../client/client_tcp.hpp"
#include "../client/client_tcp_config_json.hpp"

namespace wfc{
  
JSON_NAME2(client_tcp_object_name, "client-tcp")

class client_object_impl: public ::wfc::multiton<
  client_tcp_object_name,
  ::wfc::instance<client_tcp>,
  client_tcp_config_json
>
{};

client_object::client_object()
  : object( std::make_shared<client_object_impl>() )
{}

}
