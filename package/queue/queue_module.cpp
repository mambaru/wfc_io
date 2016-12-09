#include "queue/queue_multiton.hpp"
#include "queue_module.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace io{
  
namespace
{
  WFC_NAME2(module_name, "io-queue")

  class impl: public ::wfc::component_list<
    module_name,
    queue_multiton
  >
  {};
}

queue_module::queue_module()
  : module( std::make_shared<impl>() )
{
}

}}
