#pragma once
#include <string>
namespace wfc{

struct io_statistics_config
{
  std::string target;
  std::string time_name = "time";
  std::string size_name = "size";
};

}
