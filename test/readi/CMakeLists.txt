add_definitions("-std=c++11")
add_executable(di di.cpp)
target_link_libraries(di readi ${Boost_LIBRARIES})
add_serial_test(di)
