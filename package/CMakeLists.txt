include(BuildInfo)

SET(
  wfc_server_source_list
  server_package.cpp
  server/server_module.cpp
  server/server_tcp/server_multiton.cpp
  server/server_tcp/server_tcp.cpp
)

add_library(
  wfc_server
  ${wfc_server_source_list}
)

build_info(wfc_server server)

install(TARGETS wfc_server DESTINATION ${CMAKE_INSTALL_PREFIX})