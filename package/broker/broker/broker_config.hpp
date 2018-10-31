#pragma once

#include <string>
#include <vector>


namespace wfc{ namespace io{ 

struct broker_config
{
  struct rule
  {
    std::string target;
    std::string log;
    std::string regex;
  };
  
  std::string target;
  std::string log;
    
  std::vector<rule> rules;
};

} }
