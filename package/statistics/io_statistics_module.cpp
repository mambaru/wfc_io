
#include "io_statistics_module.hpp"
#include "statistics/io_statistics_multiton.hpp"
#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wfc{

namespace
{
  WFC_NAME2(module_name, "io-statistics")
  class impl: public ::wfc::component_list<
    module_name,
    io_statistics_multiton
  >
  {
  };
}

io_statistics_module::io_statistics_module()
  : module( std::make_shared<impl>() )
{
}

}
