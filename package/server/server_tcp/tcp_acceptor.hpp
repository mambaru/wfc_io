#pragma once
#include <iow/ip/tcp/server/acceptor.hpp>
#include "tcp_connection.hpp"


namespace wfc{

class tcp_acceptor
  : public ::iow::ip::tcp::server::acceptor<tcp_connection>
{
  typedef ::iow::ip::tcp::server::acceptor<tcp_connection> super;
public:
  typedef super::descriptor_type descriptor_type;
  tcp_acceptor(descriptor_type&& desc)
    : super( std::move(desc) )
  {}
};
  
}
