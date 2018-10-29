#pragma once

#include <boost/regex.hpp>
#include <string>

namespace wfc{ namespace io{ 

class regex_match
{
  boost::regex _reg;
  std::string  _str;
public:
  regex_match(const std::string& regex);
  
  bool match(const char* beg, const char* end);
};


}}
