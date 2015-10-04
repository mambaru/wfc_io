
#include "client_module.hpp"
#include "client_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{
  
WFC_NAME2(client_module_name, "client")

class client_module::impl: public ::wfc::component_list<
  client_module_name,
  client_multiton
>
{
};

client_module::client_module()
  : module( std::make_shared<client_module::impl>() )
{
}

}
