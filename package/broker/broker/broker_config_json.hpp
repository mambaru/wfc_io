#pragma once

#include "broker_config.hpp"
#include <wfc/json.hpp>

namespace wfc{  namespace io{ 
 
struct broker_config_json
{
  typedef broker_config::rule rule;

  JSON_NAME(target)
  JSON_NAME(regex)
  JSON_NAME(rule_log)

  typedef ::wfc::json::object<
    rule,
    json::member_list<
      json::member<n_target,   rule, std::string, &rule::target, json::string<> >,
      json::member<n_rule_log, rule, std::string, &rule::rule_log, json::string<> >,
      json::member<n_regex,    rule, std::string, &rule::regex, json::string<> >
    >,
    json::strict_mode
  > rule_json;

  typedef json::array< std::vector< rule_json > > array_rule_json;
  JSON_NAME(rules)
  JSON_NAME(target_log)

  
  typedef json::object<
    broker_config,
    json::member_list<
      json::member<n_target, broker_config, std::string, &broker_config::target >,
      json::member<n_target_log, broker_config, std::string, &broker_config::target_log>,
      json::member<n_rules, broker_config, std::vector<rule>, &broker_config::rules, array_rule_json >
    >,
    json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
