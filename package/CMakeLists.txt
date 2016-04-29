include(BuildInfo)

SET(
  wfc_io_source_list
  io_package.cpp

  server/server_module.cpp
  server/server_tcp/server_multiton.cpp
  server/server_tcp/server_tcp.cpp
  
  client/client_module.cpp
  client/client_tcp/client_multiton.cpp
  client/client_tcp/client_tcp.cpp
  client/client_tcp/client_tcp_adapter.cpp
  client/client_tcp/client_tcp_map.cpp

)

add_library(
  wfc_io
  ${wfc_io_source_list}
)

build_info(wfc_io io)

install(TARGETS wfc_io DESTINATION ${CMAKE_INSTALL_PREFIX})
