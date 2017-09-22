#pragma once

#include "client_tcp_adapter.hpp"
#include <iow/io/io_id.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/mutex.hpp>

namespace wfc{ namespace io{
 

class client_tcp_map
  : public iinterface
{
public:
  class handler_wrapper;
  
  typedef std::mutex mutex_type;
  typedef client_tcp_adapter client_type;
  typedef std::shared_ptr<client_type> client_ptr;
  typedef client_type::io_service_type io_service_type;
  typedef client_type::options_type options_type;

  client_tcp_map( io_service_type& io);
  
  void reconfigure(const options_type& opt);
  
  void stop();
  
  client_ptr find( io_id_t id ) const;

  client_ptr upsert( io_id_t id);

  // iinterface
  virtual void reg_io( io_id_t id, std::weak_ptr< ::wfc::iinterface > src) override;
  
  virtual void unreg_io( io_id_t id) override;

  virtual void perform_io( iinterface::data_ptr d, io_id_t id, output_handler_t handler) override;

private:
  
  client_ptr find_( io_id_t id ) const;
  
private:
  typedef std::map< io_id_t, client_ptr> client_map_t;

  typedef rwlock<std::mutex> mutex_type;
  io_service_type& _io;
  options_type _opt;
  client_map_t _clients;
  mutable mutex_type _mutex;
};
  
}}
