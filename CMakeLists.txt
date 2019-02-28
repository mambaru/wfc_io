cmake_minimum_required(VERSION 3.1)

project(wfc_io)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ci)

wfc_libs()

include_directories(.)

add_subdirectory(package)

if ( BUILD_TESTING )
  enable_testing()
  add_subdirectory(tests)
endif()
