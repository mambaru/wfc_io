#pragma once

#include "server_config.hpp"
#include <wfc/json.hpp>

namespace wfc{

struct server_config_json
{
  typedef json::object<
    server_config,
    fas::type_list_n<
    >::type
  > type;
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}
