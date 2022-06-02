#include "hub/hub_multiton.hpp"
#include "hub_module.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace io{
  
namespace
{
  WFC_NAME2(module_name, "io-hub")

  class impl: public ::wfc::component_list<
    module_name,hub_multiton
  >
  {};
}

hub_module::hub_module()
  : module( std::make_shared<impl>() )
{
}

}}
