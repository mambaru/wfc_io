add_executable(package_test package_suite.cpp tank_suite.cpp  package_test.cpp)
target_link_libraries(package_test PUBLIC wfc_io fas::testing)
wci_test(TARGET package_test COMMAND package_test)


