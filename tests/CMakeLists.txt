enable_testing()

add_executable(server_test server_test.cpp)
add_test(server_test core_test)
target_link_libraries(server_test wfc_io )

