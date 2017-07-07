#pragma once

#include "server_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
//#include <iow/io/aux/data_pool.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{

class server_tcp
  : public ::wfc::domain_object<iinterface, server_tcp_config>
  , public std::enable_shared_from_this<server_tcp>
{
  class impl;
  typedef ::iow::io::data_pool<data_type> data_pool;
public:
  virtual ~server_tcp();
  server_tcp();
  virtual void start() override;
  virtual void initialize() override;
  virtual void stop() override;
private:
  std::shared_ptr<impl> _impl;
  std::weak_ptr<iinterface> _target;
//  std::shared_ptr<data_pool> _data_pool;
};

}}
