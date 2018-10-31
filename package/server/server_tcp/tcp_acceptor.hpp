//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once
#include <iow/ip/tcp/server/acceptor.hpp>
#include "tcp_connection.hpp"

namespace wfc{ namespace io{

class tcp_acceptor
  : public ::iow::ip::tcp::server::acceptor<tcp_connection>
{
  typedef ::iow::ip::tcp::server::acceptor<tcp_connection> super;
public:
  typedef super::descriptor_type descriptor_type;
  explicit tcp_acceptor(descriptor_type&& desc)
    : super( std::move(desc) )
  {}
};
  
}}
