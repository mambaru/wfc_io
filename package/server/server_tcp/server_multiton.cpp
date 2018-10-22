
#include "server_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include "server_tcp.hpp"
#include "server_tcp_config_json.hpp"
#include <wfc/name.hpp>
#include <memory>

namespace wfc{ namespace io{

namespace 
{
  WFC_NAME2(component_name, "server-tcp")

  class impl: public ::wfc::multiton<
    component_name,
    ::wfc::instance<server_tcp>,
    server_tcp_config_json,
      component_features::EnableCPU
    | component_features::DisabledWorkflow
  >
  {
    virtual std::string interface_name() const override
    {
      return std::string("");
    }

    virtual std::string description() const override
    {
      return "TCP server";
    }
  };
}

server_multiton::server_multiton()
  : component( std::make_shared< impl>() )
{}

}}
