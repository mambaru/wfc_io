//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018, 2022
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/component.hpp>

namespace wfc{ namespace io{

class server_multiton
  : public ::wfc::component
{
public:
  server_multiton();
};

}}
