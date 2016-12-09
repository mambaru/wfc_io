#pragma once

#include "queue_config.hpp"
#include <wfc/workflow.hpp>
#include <wfc/domain_object.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{
  
class queue
  : public ::wfc::domain_object<iinterface, queue_config>
  , public std::enable_shared_from_this<queue>
{
  typedef std::shared_ptr< ::wfc::workflow > workflow_ptr;
public:
  queue();
// domain
  virtual void ready() override;
  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override;
  
  
  //virtual void stop(const std::string&) override;

// ijsonrpc
  /*
  virtual void perform_incoming(incoming_holder, io_id_t, rpc_outgoing_handler_t handler) override;
  virtual void perform_outgoing(outgoing_holder, io_id_t) override;
  
private:
  rpc_outgoing_handler_t make_handler_(rpc_outgoing_handler_t&& handler);
private:
  */
private:
  outgoing_handler_t make_handler_(outgoing_handler_t&& handler);
  //workflow_ptr _incoming;
  std::shared_ptr<iinterface> _target;
  workflow_ptr _callback_workflow;

  queue_config _config;
};

}}
