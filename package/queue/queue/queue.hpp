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
  virtual void ready() override;
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override;
  virtual void unreg_io(io_id_t /*io_id*/) override;
  virtual void perform_io(data_ptr d, io_id_t io_id, outgoing_handler_t handler) override;
private:
  outgoing_handler_t make_handler_(outgoing_handler_t&& handler);
  std::weak_ptr<iinterface> _target;
  workflow_ptr _callback_workflow;
  queue_config _config;
};

}}
