include(BuildInfo)

SET(
  wfc_client_source_list
  client_package.cpp
  module/client_module.cpp
  module/client_object.cpp
  client/client_tcp.cpp
)

add_library(
  wfc_client
  ${wfc_client_source_list}
)

build_info(wfc_client client)

install(TARGETS wfc_client DESTINATION ${CMAKE_INSTALL_PREFIX})