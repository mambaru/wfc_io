cmake_minimum_required(VERSION 3.1)

project(wfc_io)

include(cmake/ci.cmake)

wci_getlib(NAME wfc ${WCI_SUPERMODULE} )
enable_stat()

add_subdirectory(package)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
endif()
