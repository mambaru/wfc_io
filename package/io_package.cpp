
#include "io_package.hpp"
#include "server/server_module.hpp"
#include "client/client_module.hpp"
#include "statistics/statistics_module.hpp"
#include "queue/queue_module.hpp"
#include <wfc/module/package.hpp>
#include <wfc/module/module_list.hpp>
#include "io_build_info.h"

namespace wfc{
 
namespace {
class impl: public ::wfc::module_list<
  io_build_info,
  io::server_module,
  io::client_module,
  io::queue_module,
  io::statistics_module
>
{  
};
}

io_package::io_package()
  : package( std::make_shared<impl>() )
{
}

}
