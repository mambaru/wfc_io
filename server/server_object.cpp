
#include "server_object.hpp"
#include <wfc/module/singleton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/json.hpp>
#include "server.hpp"
#include "server_config_json.hpp"

namespace wfc{
  
JSON_NAME2(server_object_name, "server")

class server_object_impl: public ::wfc::multiton<
  server_object_name,
  wfc::instance<server>,
  server_config_json
>
{
};

server_object::server_object()
  : object( std::make_shared<server_object_impl>() )
{
}

}
