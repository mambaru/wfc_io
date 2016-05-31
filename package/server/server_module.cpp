
#include "server_module.hpp"
#include "server_tcp/server_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{

namespace
{
  WFC_NAME2(module_name, "server")
  class impl: public ::wfc::component_list<
    module_name,
    server_multiton
  >
  {
  };
}

server_module::server_module()
  : module( std::make_shared<impl>() )
{
}

}
