
#include "regex_match.hpp"

namespace wfc{ namespace io{ 

regex_match::regex_match(const std::string& regex)
  : _reg(regex)
{ 
}

bool regex_match::match(const char* beg, const char* end ) 
{
  return boost::regex_match(beg, end, _reg );
}

}}
