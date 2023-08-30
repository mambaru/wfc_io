
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

  hub/hub_module.cpp
  hub/hub/hub_multiton.cpp
  hub/hub/hub.cpp

  
  tank/tank_module.cpp
  tank/tank/tank_multiton.cpp
  tank/tank/tank_io.cpp

  queue/queue_module.cpp
  queue/queue/queue_multiton.cpp
  queue/queue/queue.cpp

  broker/broker_module.cpp
  broker/broker/broker_multiton.cpp
  broker/broker/broker.cpp
  broker/broker/regex_match.cpp
)



if ( WFC_ENABLE_STAT )
SET(
  wfc_io_source_list
  ${wfc_io_source_list}
  statistics/statistics_module.cpp
  statistics/statistics/statistics_multiton.cpp
  statistics/statistics/statistics.cpp
)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  update_ogenc(TARGETS wfc_io WARNINGS -Wnull-dereference OFF)
endif()

add_library(wfc_io ${wfc_io_source_list})
wci_targets(wfc_io)
enable_stat(wfc_io)
target_compile_definitions(wfc_io PRIVATE -DBOOST_ASIO_NO_DEPRECATED)
target_include_directories(wfc_io PUBLIC ${wfc_io_SOURCE_DIR})
target_link_libraries(wfc_io PUBLIC wfc)


build_info(TARGET wfc_io PREFIX io)

install(TARGETS wfc_io DESTINATION ${CMAKE_INSTALL_PREFIX})
