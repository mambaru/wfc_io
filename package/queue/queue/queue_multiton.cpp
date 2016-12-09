
#include "queue_multiton.hpp"
#include "queue.hpp"
#include "queue_config_json.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace io{

namespace 
{ 
  WFC_NAME2(multiton_name, "io-queue") 

  class impl: public ::wfc::multiton<
    multiton_name,
    ::wfc::instance<queue>,
    queue_config_json
  >
  {};
}


queue_multiton::queue_multiton()
  : component( std::make_shared<impl>() )
{}

}}
