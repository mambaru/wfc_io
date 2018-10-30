#pragma once

#include "broker_config.hpp"
#include "regex_match.hpp"
#include <wfc/domain_object.hpp>

#include <wfc/mutex.hpp>
#include <string>
#include <memory>

namespace wfc{ namespace io{ 
  
class broker
  : public domain_object<iinterface, broker_config, nostat>
{
  typedef domain_object<iinterface, broker_config, nostat> super;
  
public:
  typedef super::domain_config domain_config;

  virtual domain_config generate(const std::string& val) override;
  virtual void configure() override;
  virtual void reconfigure() override;
  // domain_proxy
  virtual void restart() override;
  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf) override;
  virtual void unreg_io(io_id_t io_id) override;
  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override;
private:
  
  typedef std::weak_ptr<iinterface> wtarget_t;
  struct rule_target
  {
    wtarget_t target;
    std::string log;
    std::shared_ptr<regex_match> matcher;
  };
  typedef rwlock<std::recursive_mutex> mutex_type; 
  typedef std::list<wtarget_t> target_list;
  typedef std::set<std::string> reject_list;
  typedef std::vector<rule_target> rule_list;

  wtarget_t    _target;
  std::string _target_log;
  
  target_list  _targets;
  rule_list    _rules;

  mutable mutex_type _mutex;
  bool _reconf_flag = false;
};

}}
