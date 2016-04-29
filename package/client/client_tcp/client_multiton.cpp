
#include "client_multiton.hpp"
#include "client_tcp.hpp"
#include "client_tcp_config_json.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/name.hpp>

namespace wfc{
  
namespace {
WFC_NAME2(client_tcp_object_name, "client-tcp")

class impl: public ::wfc::multiton<
  client_tcp_object_name,
  ::wfc::instance<client_tcp>,
  client_tcp_config_json
>
{};
}

client_multiton::client_multiton()
  : component( std::make_shared<impl>() )
{}

}
