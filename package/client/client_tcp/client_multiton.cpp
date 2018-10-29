
#include "client_multiton.hpp"
#include "client_tcp.hpp"
#include "client_tcp_config_json.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace io{
  
namespace 
{
  WFC_NAME2(object_name, "client-tcp")

  class impl: public ::wfc::multiton<
    object_name,
    ::wfc::instance<client_tcp>,
    client_tcp_config_json
  >
  {
    virtual std::string interface_name() const override
    {
      return std::string("wfc::iinterface");
    }

    virtual std::string description() const override
    {
      return "TCP client";
    }
  };
}

client_multiton::client_multiton()
  : component( std::make_shared<impl>() )
{}

}}
