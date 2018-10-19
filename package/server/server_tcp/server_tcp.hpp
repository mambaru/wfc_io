#pragma once

#include "server_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{

class target_wrapper;
  
class server_tcp
  : public ::wfc::domain_object<iinterface, server_tcp_config>
  , public std::enable_shared_from_this<server_tcp>
{
  class impl;
public:
  virtual ~server_tcp();
  server_tcp();
  virtual void initialize() override;
  virtual void start() override;
  virtual void restart() override;
  virtual void stop() override;
private:
  void run_();
  std::shared_ptr<impl> _impl;
  std::string _port;
  std::string _addr;
  std::shared_ptr<target_wrapper> _target;
  //std::weak_ptr<iinterface> _target;
};

}}
