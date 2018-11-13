include(BuildInfo)

SET(
  wfc_io_source_list
  io_package.cpp

  server/server_module.cpp
  server/server_tcp/server_multiton.cpp
  server/server_tcp/server_tcp.cpp
  server/server_udp/server_udp_multiton.cpp
  server/server_udp/server_udp.cpp
    
  client/client_module.cpp
  client/client_tcp/client_multiton.cpp
  client/client_tcp/client_tcp.cpp
  client/client_tcp/client_tcp_adapter.cpp
  client/client_tcp/client_tcp_map.cpp

  queue/queue_module.cpp
  queue/queue/queue_multiton.cpp
  queue/queue/queue.cpp

  broker/broker_module.cpp
  broker/broker/broker_multiton.cpp
  broker/broker/broker.cpp
  broker/broker/regex_match.cpp

)

if (WFC_ENABLE_STAT)
SET(
  wfc_io_source_list
  ${wfc_io_source_list}
  statistics/statistics_module.cpp
  statistics/statistics/statistics_multiton.cpp
  statistics/statistics/statistics.cpp
)
endif()

add_library(
  wfc_io
  ${wfc_io_source_list}
)

build_info(wfc_io io)

if ( STANDALONE )

  include(FindThreads)
  find_package(Boost COMPONENTS system program_options filesystem date_time REQUIRED)

  target_link_libraries(
    wfc_io
      wfc
      wrtstat
      wlog
      wflow
      wjrpc
      ${Boost_LIBRARIES} 
      ${CMAKE_THREAD_LIBS_INIT}
  )
endif()


install(TARGETS wfc_io DESTINATION ${CMAKE_INSTALL_PREFIX})
