
#include "server_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include "server_tcp.hpp"
#include "server_tcp_config_json.hpp"
#include <wfc/name.hpp>
#include <memory>

namespace wfc{
  
WFC_NAME2(server_tcp_multiton_name, "server-tcp")

class server_multiton_impl: public ::wfc::multiton<
  server_tcp_multiton_name,
  ::wfc::instance<server_tcp>,
  server_tcp_config_json
>
{};


server_multiton::server_multiton()
  : component( std::make_shared< server_multiton_impl>() )
{}

}
