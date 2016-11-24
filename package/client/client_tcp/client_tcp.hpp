
#pragma once

#include "client_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/workflow.hpp>
#include <string>
#include <memory>

namespace wfc{

class client_tcp_adapter;

class client_tcp
  : public ::wfc::domain_object< iinterface, client_tcp_config>
{
public:
  virtual ~client_tcp();
  client_tcp();

  // domain_object  
  //virtual void start(const std::string&) override;
  virtual void configure() override;
  virtual void initialize() override;

  virtual void stop(const std::string&) override;
  
  // iinterface
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override;

  virtual void unreg_io(io_id_t /*io_id*/) override;

  virtual void perform_io(data_ptr /*d*/, io_id_t /*io_id*/, outgoing_handler_t handler) override;

private:
  std::shared_ptr<client_tcp_adapter> _adapter;
};

}
