#pragma once

#include "client_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <string>
#include <memory>

namespace wfc{



class client_tcp
  : public ::wfc::domain_object< ijsonrpc, client_tcp_config>
{
public:
  virtual ~client_tcp();
  client_tcp();
  // domain_object
  virtual void start(const std::string&) override;
  virtual void stop(const std::string&) override;
  virtual void reconfigure() override;
  
  // iinterface
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override;

  virtual void unreg_io(io_id_t /*io_id*/) override;

  virtual void perform_io(data_ptr /*d*/, io_id_t /*io_id*/, outgoing_handler_t handler) override;

  // ijsonrpc
  virtual void perform_incoming(incoming_holder, io_id_t, outgoing_handler_t handler) override;
  
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;

private:
  class impl;
  std::shared_ptr<impl> _impl;
  std::weak_ptr<iinterface> _target;
};

}
