//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "client_tcp_adapter.hpp"
#include "client_tcp_config.hpp"
#include <iow/io/io_id.hpp>
#include <wflow/owner.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/mutex.hpp>
#include <list>
#include <map>

namespace wfc{ namespace io{


class client_tcp_map
  : public iinterface
{
  class handler_wrapper;
public:
  typedef rwlock<std::recursive_mutex> mutex_type;
  typedef client_tcp_adapter client_type;
  typedef std::shared_ptr<client_type> client_ptr;
  typedef std::weak_ptr<client_type> client_wptr;
  typedef client_type::io_context_type io_context_type;
  typedef client_tcp_config options_type;

  explicit client_tcp_map( io_context_type& io);

  void reconfigure_and_start(const options_type& opt);

  void stop();

  client_ptr find( io_id_t id ) const;

  client_ptr create(io_id_t id);
  
  void free(client_ptr cli);

  // iinterface
  virtual void reg_io( io_id_t id, std::weak_ptr< ::wfc::iinterface > src) override;

  virtual void unreg_io( io_id_t id) override;

  virtual void perform_io( iinterface::data_ptr d, io_id_t id, output_handler_t handler) override;

private:
  client_ptr upsert_( io_id_t id);

  client_ptr find_( io_id_t id ) const;

  client_ptr create_();

  void stop_all_clients();
private:
  typedef std::map< io_id_t, client_ptr> client_map_t;
  typedef std::list<client_ptr> client_list_t;
  io_context_type& _io;
  wflow::owner _owner;
  options_type _opt;
  client_map_t _clients;
  mutable mutex_type _mutex;
  client_list_t _startup_pool;
  client_list_t _primary_pool;
  client_list_t _secondary_pool;
  bool _startup_flag = true;
  bool _stop_flag = false;

};

}}
