#include "broker.hpp"
#include <wjrpc/incoming/send_error.hpp>
#include <wfc/logger.hpp>

#define BROKER_LOG(NAME, X) if (!NAME.empty()) { WFC_LOG_MESSAGE(NAME, X) }

namespace wfc{ namespace io{

broker::domain_config broker::generate(const std::string& val)
{
  domain_config conf = super::generate(val);
  if ( !val.empty() )
  {
    conf.rules.resize(1);
    conf.rules.back().target="target1";
    conf.rules.back().regex=".*";
  }
  return conf;
}

void broker::configure()
{
  _reconf_flag = false;
}

void broker::reconfigure()
{
  _reconf_flag = true;
}

void broker::restart()
{
  target_list  targets;
  rule_list    rules;

  const auto& opt = this->options();
  targets.clear();

  for (const broker_config::rule& r: opt.rules)
  {
    rules.push_back(rule_target());
    auto trg = this->get_target<iinterface>(r.target);
    rules.back().target = trg;
    targets.push_back(trg);
    rules.back().log = r.log;
    rules.back().matcher = std::make_shared<regex_match>(r.regex);
  }

  std::lock_guard<mutex_type> lk(_mutex);
  _targets = targets;
  _rules = rules;
  _target_log = opt.log;
  _target = this->get_target<iinterface>(opt.target);
}

void broker::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  super::reg_io(io_id, itf);
  read_lock<mutex_type> lk(_mutex);
  for (auto& wt : _targets )
  {
    if (auto t = wt.lock())
      t->reg_io(io_id, itf);
  }
}

void broker::unreg_io(io_id_t io_id)
{
  super::unreg_io(io_id);

  read_lock<mutex_type> lk(_mutex);
  for (auto& wt : _targets )
  {
    if (auto t = wt.lock())
      t->unreg_io(io_id);
  }
}


void broker::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
{
  read_lock<mutex_type> lk(_mutex);
  if ( this->suspended() || d==nullptr  )
  {
    if (auto t = _target.lock())
      t->perform_io(std::move(d), io_id, handler);
    return;
  }

  auto itr = std::find_if(_rules.begin(), _rules.end(), [&d](const rule_target& r){
    return r.matcher->match( d->data(), d->data() + d->size() );
  });

  if ( itr!=_rules.end() )
  {
    BROKER_LOG(itr->log, d)
    if (auto t = itr->target.lock())
    {
      t->perform_io(std::move(d), io_id, handler);
    }
    return;
  }
  /*
  for (const rule_target& r: _rules)
  {
    if ( r.matcher->match( d->data(), d->data() + d->size() ) )
    {
      BROKER_LOG(r.log, d)
      if (auto t = r.target.lock())
      {
        t->perform_io(std::move(d), io_id, handler);
      }
      return;
    }
  }*/

  BROKER_LOG(_target_log, d)
  if (auto t = _target.lock())
    t->perform_io(std::move(d), io_id, handler);
}

}}
