include(BuildInfo)

SET(
  wfc_server_source_list
  server_package.cpp
  server_module.cpp
  server_object.cpp
  server.cpp
)

add_library(
  wfc_server SHARED
  ${wfc_server_source_list}
)

build_info(wfc_server server)

install(TARGETS wfc_server DESTINATION ${CMAKE_INSTALL_PREFIX})