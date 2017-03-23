
#include "server_udp_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include "server_udp.hpp"
#include "server_udp_config_json.hpp"
#include <wfc/name.hpp>
#include <memory>

namespace wfc{ namespace io{

namespace 
{
  WFC_NAME2(component_name, "server-udp")

  class impl: public ::wfc::multiton<
    component_name,
    ::wfc::instance<server_udp>,
    server_udp_config_json
  >
  {};
}

server_udp_multiton::server_udp_multiton()
  : component( std::make_shared< impl>() )
{}

}}
