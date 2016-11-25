
#include "io_statistics_multiton.hpp"
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include "io_statistics.hpp"
#include "io_statistics_config_json.hpp"
#include <wfc/name.hpp>
#include <memory>

namespace wfc{

namespace 
{
  WFC_NAME2(component_name, "io-statistics")

  class impl: public ::wfc::multiton<
    component_name,
    ::wfc::instance<io_statistics>,
    io_statistics_config_json,
    ::wfc::component_features::DisabledPriority
    | ::wfc::component_features::DisabledWorkflow
  >
  {};
}

io_statistics_multiton::io_statistics_multiton()
  : component( std::make_shared< impl>() )
{}

}
