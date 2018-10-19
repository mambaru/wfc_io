//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <iostream>
#include "tcp_acceptor.hpp"
#include "tcp_connection.hpp"
#include <iow/ip/tcp/acceptor/options.hpp>
#include <iow/ip/tcp/server/options.hpp>
#include <iow/ip/tcp/connection/options.hpp>
#include <wfc/iinterface.hpp>

namespace wfc{ namespace io{

struct tcp_connection_options
  : ::iow::ip::tcp::connection::options
{
  std::weak_ptr<iinterface> target;
  bool direct_mode = false;
};

typedef ::iow::ip::tcp::acceptor::options< tcp_connection, tcp_connection_options > tcp_acceptor_options;
typedef ::iow::ip::tcp::server::options< tcp_acceptor_options > tcp_options;
  
}}
