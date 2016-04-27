add_executable(nestsynapse synapse.cpp)
target_link_libraries(nestsynapse synapse ${Boost_LIBRARIES})
add_serial_test(nestsynapse)