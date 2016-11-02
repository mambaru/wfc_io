
#include "io_package.hpp"
#include "server/server_module.hpp"
#include "client/client_module.hpp"
#include "statistics/io_statistics_module.hpp"
#include <wfc/module/package.hpp>
#include <wfc/module/module_list.hpp>
#include "io_build_info.h"

namespace wfc{
 
namespace {
class impl: public ::wfc::module_list<
  io_build_info,
  server_module,
  client_module,
  io_statistics_module
>
{  
};
}

io_package::io_package()
  : package( std::make_shared<impl>() )
{
}

}
