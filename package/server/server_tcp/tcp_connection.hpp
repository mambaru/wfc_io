#pragma once

#include <iow/ip/tcp/connection/connection.hpp>
#include <memory>
#include <mutex>
#include <wfc/iinterface.hpp>
#include <wfc/logger.hpp>

namespace wfc{ namespace io{

class tcp_connection
  : public ::iow::ip::tcp::connection::connection_base<>
  , public std::enable_shared_from_this<tcp_connection>
  , public iinterface
{
public:
  typedef ::iow::ip::tcp::connection::connection_base<> super;
  typedef super::descriptor_type descriptor_type;
  typedef super::io_id_type io_id_type;
  typedef super::output_handler_type output_handler_type;
  typedef super::mutex_type mutex_type;
  typedef super::data_ptr data_ptr;
  
  tcp_connection(descriptor_type&& desc)
    : super( std::move(desc) )
  {}
  
  void reset()
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::reset_(*this);
  }

  void start()
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    this->start_(*this);
  }

  template<typename O>
  void start(O&& opt)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::start_(*this, std::forward<O>(opt));
  }

  template<typename O>
  void reconfigure(O&& opt)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::reconfigure_(*this, std::forward<O>(opt));
  }

  template<typename O>
  void initialize(O opt)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    auto wtarget = opt.target;
    auto startup_handler = opt.startup_handler;
    auto shutdown_handler = opt.shutdown_handler;
    std::weak_ptr<tcp_connection> wthis = this->shared_from_this();
    
    if ( !opt.direct_mode) 
    {
      opt.startup_handler = [wtarget, wthis, startup_handler](io_id_type id, output_handler_type outgoing)
      {
        if ( auto ptarget = wtarget.lock() )
        {
          ptarget->reg_io(id, wthis);
        }
        if ( startup_handler!=nullptr )
        {
          startup_handler(id, outgoing);
        }
      };

      opt.shutdown_handler = [wtarget, shutdown_handler](io_id_type id)
      {
        if ( auto ptarget = wtarget.lock() )
        {
          ptarget->unreg_io(id);
        }
        
        if ( shutdown_handler!=nullptr )
        {
          shutdown_handler(id);
        }
      };
    }
    
    super::initialize_(*this, std::forward<O>(opt));
  }

  
  void close()
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    this->close_(*this);
  }
  
  void stop()
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::stop_(*this);
  }

  template<typename Handler>
  void shutdown(Handler&& handler)
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    super::shutdown_(*this, std::forward<Handler>(handler));
  }
  
  virtual void reg_io(io_id_t /*io_id*/, std::weak_ptr<iinterface> /*itf*/) override
  {
  }

  virtual void unreg_io(io_id_t /*io_id*/) override
  {
  }

  virtual void perform_io(data_ptr d, io_id_t /*io_id*/, output_handler_t /*handler*/) override
  {
    std::lock_guard< mutex_type > lk( super::mutex() );
    if ( auto outgoing = super::get_aspect().get< iow::io::descriptor::_context_>().output_handler )
    {
      outgoing( std::move(d) );
    }
  }

};

}}
