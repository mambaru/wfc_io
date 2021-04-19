
#include "io_package.hpp"
#include "server/server_module.hpp"
#include "client/client_module.hpp"
#include "broker/broker_module.hpp"
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
    io::broker_module
  #ifdef WFC_ENABLE_STAT
    ,io::statistics_module
  #endif
  >
  {
    virtual std::string description() const override
    {
      return "Package of IP/TCP and I/O modules";
    }

    virtual int show_order() const override
    {
      return 500;
    }

  };
}

io_package::io_package()
  : package( std::make_shared<impl>() )
{
}

}
