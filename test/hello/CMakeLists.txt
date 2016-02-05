add_executable(world hello.cpp)
target_link_libraries(world hello ${Boost_LIBRARIES})
add_serial_test(world)