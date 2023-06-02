#pragma once

#include "tank_options.hpp"
#include <wfc/tank/tank_domain.hpp>
#include <wfc/tank/tank_gun.hpp>
#include <wfc/mutex.hpp>
#include <wfc/name.hpp>
#include <string>
#include <memory>
#include <map>
#include <limits>
#include <deque>

namespace wfc{ namespace io{

class tank_io
  : public tank_domain<iinterface, tank_config<iinterface::data_type, iinterface::data_type, tank_options> >
  , public std::enable_shared_from_this<tank_io>
{
  typedef tank_domain<iinterface, tank_config<iinterface::data_type, iinterface::data_type, tank_options> > super;
  typedef tank_config<data_type, data_type, tank_options> options_type_t;
  typedef tank_gun<iinterface, options_type_t> tank_gun_t;
  typedef std::shared_ptr<tank_gun_t> tank_gun_ptr;
public:
  tank_io();
  virtual void configure() override;
  virtual void stop() override;

private:

  typedef class __counter
  {
  public:
    __counter() = default;
    __counter(long c, long f, long t, long s) noexcept
      : current(c), from(f), to(t), step(s){}
    long get() const { return current;}
    long next()
    {
      if ( to - current > step )
        current += step;
      else
        current = from + step - (to - current);
      return current;
    }
  private:
    long current = 0;
    long from = 0;
    long to = std::numeric_limits<long>::max();
    long step = 1;
  } counter_t;
  typedef std::shared_ptr<counter_t> counter_ptr;
  typedef std::map<std::string, counter_ptr> counter_map_t;
private:
  tank_gun_ptr _tank_gun;
  bool make_request_(const tank_options& opt, data_type& req, data_type& res);
  bool check_response_(const tank_options& opt, const data_type& req, const data_type& res1, const data_type& res2);
  static std::atomic_int _call_id;
  std::atomic_bool _initial_make;

  typedef std::mutex mutex_type;
  mutable mutex_type _mutex;
  std::map<int, std::string> _call_map;
  std::deque<std::string> _io_queue;
  counter_map_t _counters;
};

}}
