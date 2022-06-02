//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "queue_config.hpp"
#include <wfc/json.hpp>
#include <wfc/core/workflow_options_json.hpp>

namespace wfc{ namespace io{
  
 
struct queue_config_json
{
  JSON_NAME(target)
  JSON_NAME(callback_workflow)
  JSON_NAME(callback_queue)
  typedef json::object<
    queue_config,
    json::member_list<
      json::member<n_target,   queue_config, std::string, &queue_config::target >,
      json::member<n_callback_queue, queue_config, bool, &queue_config::callback_queue>,
      json::member<n_callback_workflow, queue_config, std::string, &queue_config::callback_workflow, wjson::raw_value<> >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
