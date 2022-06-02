#pragma once

#include "tank_options.hpp"
#include <wjson/wjson.hpp>

namespace wfc{  namespace io{ 
 
struct tank_options_json
{
  JSON_NAME(enable_vars)
  JSON_NAME(show_request)
  JSON_NAME(show_response)
  JSON_NAME(check_response)
  JSON_NAME(jsonrpc_method)

  typedef wjson::object<
    tank_options,
    wjson::member_list<
      wjson::member<n_enable_vars, tank_options, bool, &tank_options::enable_vars>,
      wjson::member<n_show_request, tank_options, bool, &tank_options::show_request>,
      wjson::member<n_show_response, tank_options, bool, &tank_options::show_response>,
      wjson::member<n_check_response, tank_options, bool, &tank_options::check_response>,
      wjson::member<n_jsonrpc_method, tank_options, std::shared_ptr<std::string>, 
                    &tank_options::jsonrpc_method, 
                    wjson::pointer<std::shared_ptr<std::string>, wjson::value<std::string> > >
    >,
    wjson::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
