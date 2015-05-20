
#include "server_package.hpp"
#include "server_module.hpp"
#include <wfc/module/module_list.hpp>
#include <iow/json/name.hpp>

namespace wfc{
  
JSON_NAME2(server_package_name, "server")

class server_package_impl: public ::wfc::module_list<
  server_package_name,
  server_module
>
{  
};

server_package::server_package()
  : package( std::make_shared<server_package_impl>() )
{
}

}
