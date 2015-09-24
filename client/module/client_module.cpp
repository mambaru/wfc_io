
#include "client_module.hpp"
#include "client_object.hpp"
#include <wfc/module/object_list.hpp>
#include <wfc/name.hpp>

namespace wfc{
  
WFC_NAME2(client_module_name, "client")

class client_module::impl: public ::wfc::object_list<
  client_module_name,
  client_object
>
{
};

client_module::client_module()
  : module( std::make_shared<client_module::impl>() )
{
}

}
