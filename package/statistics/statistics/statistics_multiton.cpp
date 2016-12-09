
#include "statistics_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include "statistics.hpp"
#include "statistics_config_json.hpp"
#include <wfc/name.hpp>
#include <memory>

namespace wfc{ namespace io{

namespace 
{
  WFC_NAME2(component_name, "io-statistics")

  class impl: public ::wfc::multiton<
    component_name,
    ::wfc::instance<statistics>,
    statistics_config_json,
    ::wfc::component_features::DisabledPriority
    | ::wfc::component_features::DisabledWorkflow
  >
  {};
}

statistics_multiton::statistics_multiton()
  : component( std::make_shared< impl>() )
{}

}}
