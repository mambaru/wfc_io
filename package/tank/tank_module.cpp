#include "tank/tank_multiton.hpp"
#include "tank_module.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace io{
  
namespace
{
  WFC_NAME2(module_name, "io-tank")

  class impl: public ::wfc::component_list<
    module_name,tank_multiton
  >
  {};
}

tank_module::tank_module()
  : module( std::make_shared<impl>() )
{
}

}}
