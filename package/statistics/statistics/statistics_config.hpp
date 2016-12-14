#pragma once
#include <string>

namespace wfc{ namespace io{

struct statistics_config
{
  std::string target;
  time_t interval_ms = 1000;
  std::string io_name = "connections";
  std::string time_name = "time";
  std::string read_name = "read_size";
  std::string write_name = "write_size";
};

} }
