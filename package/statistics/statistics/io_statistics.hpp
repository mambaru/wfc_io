#pragma once

#include "io_statistics_config.hpp"
#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/statistics/statistics.hpp>
#include <string>
#include <memory>

namespace wfc{

class io_statistics
  : public ::wfc::domain_object<iinterface, io_statistics_config>
{
public:
  virtual void initialize() override;
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override;
  virtual void unreg_io(io_id_t /*io_id*/) override;
  virtual void perform_io(data_ptr /*d*/, io_id_t /*io_id*/, outgoing_handler_t handler) override;
private:
  std::weak_ptr<iinterface> _target;
  statistics::meter_ptr _total_meter;
  statistics::meter_ptr _input_meter;
  statistics::meter_ptr _output_meter;
};

}
