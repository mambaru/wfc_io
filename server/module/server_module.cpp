
#include "server_module.hpp"
#include "server_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/json.hpp>

namespace wfc{
  
JSON_NAME2(server_module_name, "server")

class server_module_impl: public ::wfc::component_list<
  server_module_name,
  server_multiton
>
{
};

server_module::server_module()
  : module( std::make_shared<server_module_impl>() )
{
}

}
