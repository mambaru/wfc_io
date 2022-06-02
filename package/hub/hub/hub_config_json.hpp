#pragma once

#include "hub_config.hpp"
#include <wjson/wjson.hpp>

namespace wfc{  namespace io{ 
 
struct hub_config_json
{
  JSON_NAME(target)

  typedef wjson::object<
    hub_config,
    wjson::member_list<
      wjson::member<n_target, hub_config, std::string, &hub_config::target >
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
