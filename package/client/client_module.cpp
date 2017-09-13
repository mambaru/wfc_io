
#include "client_module.hpp"
#include "client_tcp/client_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{ namespace io{

namespace
{
  WFC_NAME2(module_name, "client")

  class impl: public ::wfc::component_list<
    module_name,
    client_multiton
  >
  {
    virtual std::string description() const override
    {
      return "Client components";
    }
  };
}

client_module::client_module()
  : module( std::make_shared<impl>() )
{
}

}}
