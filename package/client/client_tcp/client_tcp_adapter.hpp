#pragma once

#include <wfc/memory.hpp>
#include <wfc/iinterface.hpp>

#include <iow/ip/tcp/client/options.hpp>
#include <iow/asio.hpp>


namespace wfc{
  
class client_tcp_adapter
  : public ::wfc::iinterface
  , public std::enable_shared_from_this< client_tcp_adapter >
{
  class impl;
public:
  typedef impl client_type;
  typedef std::shared_ptr<client_type> client_ptr;

  typedef ::iow::ip::tcp::client::options options_type;
  typedef ::iow::asio::io_service io_service_type;
  
  client_tcp_adapter( io_service_type& io, std::weak_ptr< iinterface > hld);
  
  ~client_tcp_adapter() ;
  
  void stop();
  
  void start( options_type opt);
  
  void set_holder(std::weak_ptr<iinterface> hld);
  
  std::weak_ptr<iinterface> get_holder() const;

  // iinterface
  virtual void perform_io( iinterface::data_ptr d, io_id_t /*io_id*/, outgoing_handler_t handler) override;

private:
  io_id_t _id;
  std::weak_ptr<iinterface> _holder;
  client_ptr _client;
};

}
