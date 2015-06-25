
#include "client_package.hpp"
#include "module/client_module.hpp"
#include <wfc/module/module_list.hpp>
#include "client_build_info.h"

namespace wfc{
  
class client_package_impl: public ::wfc::module_list<
  client_build_info,
  client_module
>
{  
};

client_package::client_package()
  : package( std::make_shared<client_package_impl>() )
{
}

}
