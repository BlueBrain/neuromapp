#compression test
add_executable(comp_test compression.cpp) 
target_link_libraries(comp_test ${Boost_LIBRARIES})
add_serial_test(comp_test)
