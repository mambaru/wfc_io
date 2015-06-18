
#include "server_package.hpp"
#include "module/server_module.hpp"
#include <wfc/module/module_list.hpp>
#include "server_build_info.h"

namespace wfc{
  
class server_package_impl: public ::wfc::module_list<
  server_build_info,
  server_module
>
{  
};

server_package::server_package()
  : package( std::make_shared<server_package_impl>() )
{
}

}
