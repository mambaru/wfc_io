#pragma once

#include <string>
#include <vector>


namespace wfc{ namespace io{ 

struct broker_config
{
  struct rule
  {
    std::string target;
    std::string rule_log;
    std::string regex;
  };
  
  std::string target;
  std::string target_log;
    
  std::vector<rule> rules;
};

} }
