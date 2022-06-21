#include "tank_io.hpp"
#include <wfc/tank/logger.hpp>
#include <wfc/core/vars.hpp>
#include <wjrpc/outgoing/outgoing_request.hpp>
#include <wjrpc/outgoing/outgoing_request_json.hpp>
#include <wjrpc/incoming/incoming_holder.hpp>
#include <wjson/error.hpp>
#include <iostream>

namespace wfc{ namespace io{
  
std::atomic_int tank_io::_call_id(0);

tank_io::tank_io(): _initial_make(false)
{

}

void tank_io::configure() 
{
  super::configure();
  _initial_make = true;
  using namespace std::placeholders;
  _tank_gun = std::make_shared<tank_gun_t>( this->owner(), &iinterface::perform_io, 
                                            std::bind(&tank_io::make_request_, this, _1, _2, _3), 
                                            std::bind(&tank_io::check_response_, this, _1, _2, _3, _4));
  this->reg_object("wonderwaffel", this->name(), _tank_gun );
}

void tank_io::stop()
{
  super::stop();
  //this->reg_object("wonderwaffel", this->name(), nullptr );
  //_tank_gun = nullptr;
}

bool tank_io::make_request_(const tank_options& opt, data_type& req, data_type& res)
{
  if ( opt.jsonrpc_method != nullptr && !opt.jsonrpc_method->empty() )
  {
    //typedef wfc::iinterface::data_type data_type;
    typedef wjrpc::outgoing_request<data_type> jrpc_request_t;
    typedef wjrpc::outgoing_request_json<wjson::raw_value<data_type> > jrpc_request_json;
    
    ++_call_id;
    
    jrpc_request_t jreq;
    jreq.method = *opt.jsonrpc_method;
    jreq.params = std::make_unique<data_type>( req.begin(), req.end() );
    jreq.id = _call_id;
    
    req.clear();
    jrpc_request_json::serializer()( jreq, std::back_inserter(req) );
    std::lock_guard<mutex_type> lk(_mutex);
    _call_map[_call_id] = std::string( res.begin(), res.end() );
  }
  else
  {
    std::lock_guard<mutex_type> lk(_mutex);
    _io_queue.push_back( std::string(res.begin(), res.end()) );
  }

  if ( opt.enable_vars )
  {
    vars::vars_list_t var_list;
    vars _vars;
    _vars.parse_text(std::string(req.begin(), req.end()));
    _vars.search_vars(&var_list);

    if ( !_initial_make )
    {
      for ( auto& cv : _counters )
      {
        if ( cv.second != nullptr)
          cv.second->next();
        else
        {
          COMMON_LOG_FATAL("TANK_IO COUNT cv.first=" << cv.first)
        }
      }
    }

    vars::replace_list_t rpls_list;
    for ( const auto& cv: var_list)
    {
      if ( cv.prefix == "TANK")
      {
        if ( cv.name == "ID" )
        {
          rpls_list.push_back({"TANK", "ID", std::to_string(_call_id)});
        }
        else if (cv.name.substr(0,5) == "COUNT")
        {
          if ( _initial_make )
          {
            if ( !cv.options.empty() )
            {
              _counters[cv.name] = std::make_shared<counter_t>(
                vars::get_opt<long>(cv, "from", 1),
                vars::get_opt<long>(cv, "from", 1),
                vars::get_opt<long>(cv, "to", 100),
                vars::get_opt<long>(cv, "step", 1)
              );
            }
            else if ( _counters.count(cv.name) == 0 )
            {
              _counters[cv.name] = std::make_shared<counter_t>();
            }
          }
          if ( _counters.count(cv.name) != 0 && _counters[cv.name]!=nullptr )
            rpls_list.push_back({"TANK", cv.name, std::to_string(_counters[cv.name]->get())});
          else
            COMMON_LOG_FATAL("TANK_IO COUNT name=" << cv.name)
        }
      }
    }
    _vars.replace_vars(rpls_list);
    req.assign(_vars.result().begin(), _vars.result().end() );
  }
  
  if (opt.show_request)
  {
    TANK_LOG_MESSAGE("IO request [" << std::string(req.begin(), req.end()) << "]")
  }
  _initial_make = false;
  return true;
}

bool tank_io::check_response_(const tank_options& opt, const data_type&, const data_type& , const data_type& res2)
{
  bool check_result = true;
  if (opt.show_response)
  {
    TANK_LOG_MESSAGE("IO response [" << std::make_unique<data_type>(res2) << "]")
  }

  if ( opt.check_response )
  {
    if ( opt.jsonrpc_method != nullptr && !opt.jsonrpc_method->empty() )
    {
      wjrpc::incoming_holder jrpc( std::move(res2) );
      wjson::json_error e;
      data_ptr d = jrpc.parse(&e);
      do if ( !e )
      {
        bool current_check = false;
        if ( jrpc.is_response() )
        {
          int id = jrpc.get_id< int >(&e);
          if ( !e )
          {
            std::unique_ptr<std::string> pres = jrpc.get_result< wjson::raw_value<std::string> >(&e);
            if ( !e && pres!=nullptr)
            {
              std::lock_guard<mutex_type> lk(_mutex);
              auto itr =  _call_map.find(id);
              if ( itr != _call_map.end() )
              {
                current_check = (itr->second == *pres);
                if ( !current_check )
                {
                  TANK_LOG_ERROR("TANK_IO JSONRPC check result fail: " << itr->second << "!=" << *pres)
                }
                _call_map.erase(itr);
              }
              else
              {
                TANK_LOG_ERROR("TANK_IO JSONRPC: Unknown call id: " << id )
              }
            }
            else
            {
              TANK_LOG_ERROR("TANK_IO JSONRPC: Invalid result: " << jrpc.result_error_message(e) )
            }
          }
          else
          {
            TANK_LOG_ERROR("TANK_IO JSONRPC: Invalid call id: " << jrpc.id_error_message(e) )
          }
        }
        else
        {
          TANK_LOG_ERROR("TANK_IO JSONRPC: " << " wait result on request. Received : " << jrpc.str() )
        }

        check_result &= current_check;
        jrpc.attach(std::move(d));
        d = jrpc.parse(&e);
      }
      else
        break;
      while(d!=nullptr);// while
      
      if ( check_result && e )
      {
        check_result = false;
        TANK_LOG_ERROR("TANK_IO JSONRPC: " << jrpc.error_message(e) )
      }
    } 
    else // if not jsonrpc         
    {
      std::lock_guard<mutex_type> lk(_mutex);
      if (!_io_queue.empty())
      {
        std::string res1 = _io_queue.front();
        std::string res2str = std::string(std::begin(res2), std::end(res2));
        _io_queue.pop_front();
        check_result = (res1 == res2str); 
        if ( !check_result ) 
        {
          TANK_LOG_ERROR("TANK_IO check result fail: " << res1 << "!=" << res2str)
        }
      }
    }
  } // if check_result
  return check_result;
}


}}
