#pragma once
#include <string>
namespace wfc{

struct io_statistics_config
{
  std::string target;
  std::string total_time_name = "time";
  std::string total_size_name = "size";
  std::string input_time_name = "read_time";
  std::string input_size_name = "read_size";
  std::string output_time_name = "write_time";
  std::string output_size_name = "write_size";
};

}
