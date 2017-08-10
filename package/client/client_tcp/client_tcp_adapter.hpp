#pragma once

#include <wfc/mutex.hpp>
#include <wfc/memory.hpp>
#include <wfc/iinterface.hpp>

#include <iow/ip/tcp/client/options.hpp>
#include <iow/asio.hpp>

#include <memory>
#include <atomic>

namespace wfc{ namespace io{
  
class client_tcp_adapter
  : public ::wfc::iinterface
  , public std::enable_shared_from_this< client_tcp_adapter >
{
  class handler_wrapper;
  class impl;
  typedef ::wfc::rwlock<std::mutex> mutex_type;
public:

  typedef impl client_type;
  typedef std::shared_ptr<client_type> client_ptr;

  typedef ::iow::ip::tcp::client::options options_type;
  typedef ::iow::asio::io_service io_service_type;
  
  client_tcp_adapter( io_service_type& io);
  
  ~client_tcp_adapter() ;
  
  void stop();
  
  void start( options_type opt);
  
  std::shared_ptr<iinterface> get_holder() const;
  
  // iinterface
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/);
  virtual void unreg_io(io_id_t /*io_id*/);
  virtual void perform_io( iinterface::data_ptr d, io_id_t /*io_id*/, output_handler_t handler) override;
  
private:
  io_service_type& _io;
  io_id_t _id;
  std::atomic<io_id_t> _holder_id;
  std::weak_ptr<iinterface> _holder;
  std::shared_ptr<handler_wrapper> _wrapper;
  client_ptr _client;
  mutable mutex_type _mutex;
};

}}
