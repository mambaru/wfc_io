#include <iostream>
#include "client_tcp.hpp"
#include "client_tcp_map.hpp"
#include <wfc/logger.hpp>
#include <iow/ip/tcp/client/client.hpp>
#include <iow/io/io_id.hpp>
#include <wfc/memory.hpp>

namespace wfc{ namespace io{


client_tcp::client_tcp()
  : _client_started(false)
{
}

void client_tcp::initialize()
{
  _client_map = std::make_shared<client_tcp_map>( this->get_workflow()->get_io_context() );
}

void client_tcp::start()
{
  if ( !this->suspended() && this->options().addr.empty() )
  {
    DOMAIN_LOG_FATAL("In the configuration of the '" << this->name() 
      <<"', you must set the 'addr' field or enable suspend mode")
    return;
  }
  this->reconfigure_and_start_();
}

void client_tcp::restart()
{
  this->reconfigure_and_start_();
}

void client_tcp::stop()
{
  _client_started = false;
  if ( _client_map!=nullptr )
  {
    _client_map->stop();
  }
}

void client_tcp::reg_io(io_id_t io_id, std::weak_ptr<iinterface> itf)
{
  if ( !this->suspended() && _client_map!=nullptr )
  {
    if ( _client_started )
      _client_map->reg_io( io_id, itf);
    else
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _wait_list[io_id] = itf;
    }
  }
}

void client_tcp::unreg_io(io_id_t io_id)
{
  if ( _client_map!=nullptr )
  {
    if ( _client_started )
      _client_map->unreg_io(io_id);
    else
    {
      std::lock_guard<mutex_type> lk(_mutex);
      _wait_list.erase(io_id);
    }
  }
}

void client_tcp::perform_io(data_ptr d, io_id_t io_id, output_handler_t handler)
{
  if ( this->suspended() )
  {
    if ( handler!= nullptr )
      handler(nullptr);
  }
  else if ( _client_map!=nullptr )
  {
    _client_map->perform_io( std::move(d), io_id, std::move(handler) );
  }
}

void client_tcp::reconfigure_and_start_()
{
  auto opt = this->options();
  auto stat_opt = this->statistics_options();

  std::weak_ptr<wflow::workflow> wcommon = this->get_common_workflow();
  opt.args.delayed_handler= [wcommon](std::chrono::milliseconds delay, std::function<void()> handler)
  {
    if (auto pflow = wcommon.lock() )
    {
      pflow->safe_post(delay, handler);
    }
  };
  
  if ( opt.rn )
  {
    if ( opt.connection.reader.sep.empty() ) opt.connection.reader.sep = "\r\n";
    if ( opt.connection.writer.sep.empty() ) opt.connection.writer.sep = "\r\n";
  }

  _client_map->reconfigure_and_start( opt );
  
  if (!_client_started )
  {
    _client_started = true;
    std::lock_guard<mutex_type> lk(_mutex);
    for ( const auto &item : _wait_list )
    {
      this->reg_io(item.first, item.second);
    }
  }

  if ( auto stat = this->get_statistics() )
  {
    if ( stat_opt.tracking_ms > 0 )
    {
      std::vector<  wfc::value_meter> meters = {
          stat->create_value_meter(stat_opt.connections),
          stat->create_value_meter(stat_opt.rchunk_count),
          stat->create_value_meter(stat_opt.rchunk_count_capacity),
          stat->create_value_meter(stat_opt.rtotal_size),
          stat->create_value_meter(stat_opt.rtotal_capacity),
          stat->create_value_meter(stat_opt.wchunk_count),
          stat->create_value_meter(stat_opt.wchunk_count_capacity),
          stat->create_value_meter(stat_opt.wtotal_size),
          stat->create_value_meter(stat_opt.wtotal_capacity)
      };
      auto pmeters = std::make_shared<std::vector<wfc::value_meter>>(std::move(meters));

      std::weak_ptr<statistics_type> wstat = stat;
      _timer_id = this->get_workflow()->create_timer(
        std::chrono::milliseconds(stat_opt.tracking_ms),
        this->wrap([this, wstat, stat_opt, pmeters]()->bool
        {
          if ( auto pstat = wstat.lock() )
          {
            /*auto set_stat=[pstat, pmeters](const std::string name, size_t value, const std::vector<size_t>& data = {} )
            {

              wrtstat::reduced_data rd;
              rd.count = value;
              if ( !data.empty() )
              {
                std::transform(data.begin(), data.end(), std::back_inserter(rd.data),
                               [](const size_t v)->time_t { return static_cast<time_t>(v);} );
              }
              pstat->add(name, std::move(rd));
            };*/

            auto cli_stat = _client_map->get_stat();
            pmeters->at(0).create(0, cli_stat.connection_count);

            pmeters->at(1).create(0, cli_stat.reader.chunk_count);
            pmeters->at(2).create(0, cli_stat.reader.chunk_count_capacity);

            /*if (cli_stat.reader.chunk_sizes.empty())
              for (auto i: cli_stat.reader.chunk_sizes) pmeters->at(3).create(i, cli_stat.reader.total_size);
            else */pmeters->at(3).create( static_cast<wrtstat::value_type>(cli_stat.reader.total_size), static_cast<wrtstat::size_type>(1) );

            /*if (cli_stat.reader.chunk_capacities.empty())
              for (auto i: cli_stat.reader.chunk_capacities) pmeters->at(4).create(i, cli_stat.reader.total_capacity);
            else */pmeters->at(4).create( static_cast<wrtstat::value_type>(cli_stat.reader.total_capacity), static_cast<wrtstat::size_type>(1));

            pmeters->at(5).create(0, cli_stat.writer.chunk_count);
            pmeters->at(6).create(0, cli_stat.writer.chunk_count_capacity);

            //std::cout << cli_stat.writer.total_size << " "<< cli_stat.writer.total_capacity << std::endl;
            /*if ( cli_stat.writer.chunk_sizes.empty() )
              for (auto i: cli_stat.writer.chunk_sizes) pmeters->at(7).create(i, cli_stat.writer.total_size);
            else*/ pmeters->at(7).create( static_cast<wrtstat::value_type>(cli_stat.writer.total_size), static_cast<wrtstat::size_type>(1));
            /*if ( cli_stat.writer.chunk_capacities.empty() )
              for (auto i: cli_stat.writer.chunk_capacities) pmeters->at(8).create(i, cli_stat.writer.total_capacity);
            else*/ pmeters->at(8).create( static_cast<wrtstat::value_type>(cli_stat.writer.total_capacity), cli_stat.writer.total_capacity);

            return true;
          }
          return false;
        }, nullptr)
      ); //_timer_id
    }
  } //stat

}

}}
