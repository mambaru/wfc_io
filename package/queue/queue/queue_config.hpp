//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>
#include <vector>
#include <wfc/core/workflow_options.hpp>

namespace wfc{ namespace io{

struct queue_config
{
  std::string target;
  // отслеживает закрытие соединения и не обрабатывает запрос, если отвалился 
  bool connection_tracking = false;

  // Очередь для ответов (опционально)
  bool callback_queue = false;
  std::string callback_workflow;
};

}}
