include(BuildInfo)

SET(
  wfc_client_source_list
  client_package.cpp
  client/client_module.cpp
  client/client_tcp/client_multiton.cpp
  client/client_tcp/client_tcp.cpp
)

add_library(
  wfc_client
  ${wfc_client_source_list}
)

build_info(wfc_client client)

install(TARGETS wfc_client DESTINATION ${CMAKE_INSTALL_PREFIX})