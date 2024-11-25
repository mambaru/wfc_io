//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/mutex.hpp>
#include <wfc/memory.hpp>
#include <wfc/iinterface.hpp>

#include <iow/ip/tcp/client/options.hpp>
#include <iow/asio.hpp>
#include <iow/io/stat.hpp>

#include "client_tcp_config.hpp"
#include <memory>
#include <atomic>

namespace wfc{ namespace io{

class client_tcp_adapter
  : public ::wfc::iinterface
  , public std::enable_shared_from_this< client_tcp_adapter >
{
  class perform_holder;
  class impl;
  typedef ::wfc::rwlock<std::recursive_mutex> mutex_type;
public:

  typedef impl client_type;
  typedef std::shared_ptr<client_type> client_ptr;

  typedef client_tcp_config options_type;
  typedef boost::asio::io_context io_context_type;

  explicit client_tcp_adapter( io_context_type& io);

  ~client_tcp_adapter() ;

  void stop();

  void start( options_type opt);

  std::shared_ptr<iinterface> get_holder() const;

  // iinterface
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void unreg_io(io_id_t io_id) override;
  virtual void perform_io( iinterface::data_ptr d, io_id_t io_id, output_handler_t handler) override;

  iow::io::connection_stat get_stat() const;

private:
  io_context_type& _io;
  io_id_t _id = 0;
  std::atomic<io_id_t> _holder_id;
  std::atomic_bool _error_flag;
  std::weak_ptr<iinterface> _holder;
  std::shared_ptr<perform_holder> _perform_holder;
  client_ptr _client;
  std::atomic_bool _keep_alive;
  mutable mutex_type _mutex;
  options_type _options;
};

}}
