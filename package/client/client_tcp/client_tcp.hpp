//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include "client_tcp_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/workflow.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{

class client_tcp_map;

class client_tcp
  : public ::wfc::domain_object< iinterface, client_tcp_config>
{
public:
  // domain_object  
  virtual void configure() override;
  virtual void initialize() override;
  virtual void stop() override;
  
  // iinterface
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override;

  virtual void unreg_io(io_id_t /*io_id*/) override;

  virtual void perform_io(data_ptr /*d*/, io_id_t /*io_id*/, output_handler_t handler) override;

private:
  std::shared_ptr<client_tcp_map> _adapter;
};

}}
