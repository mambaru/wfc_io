
#include "server_module.hpp"
#include "server_object.hpp"
#include <wfc/module/object_list.hpp>
#include <wfc/json.hpp>

namespace wfc{
  
JSON_NAME2(server_module_name, "server")

class server_module::impl: public ::wfc::object_list<
  server_module_name,
  server_object
>
{
};

server_module::server_module()
  : module( std::make_shared<server_module::impl>() )
{
}

}
