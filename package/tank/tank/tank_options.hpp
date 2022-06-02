#pragma once

#include <string>
#include <memory>

namespace wfc{ namespace io{ 

struct tank_options
{
  bool enable_vars = false;
  bool show_request = false;
  bool show_response = false;
  bool check_response = false;
  // Если указан, то request/response это parans и result в запросе, id генерится автоматически
  std::shared_ptr<std::string> jsonrpc_method = nullptr;

};

} }
