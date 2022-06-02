
#include "tank_multiton.hpp"
#include "tank_io.hpp"
#include "tank_options_json.hpp"


#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include <wfc/name.hpp>
#include <wfc/tank/tank_config_json.hpp>

namespace wfc{ namespace io{ 

namespace { 
  
  WFC_NAME2(component_name, "io-tank") 

  class impl: public ::wfc::multiton<
    component_name,
    ::wfc::instance<tank_io>,
    tank_config_json<wjson::raw_value<iinterface::data_type>, wjson::raw_value<iinterface::data_type>, tank_options_json>,
    component_features::DisabledWorkflow /*| component_features::DisabledPriority*/,
    stat_tank_config_json
  >
  {
    virtual std::string interface_name() const override
    {
      return std::string("wfc::iinterface");
    }

    virtual std::string description() const override
    {
      return "io tank of methods";
    }
  };
}


tank_multiton::tank_multiton()
  : ::wfc::component( std::make_shared<impl>() )
{}

}}
