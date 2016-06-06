#list of tests
set(nest_tests synapse)

#loop over nest test for creation
foreach(i ${nest_tests})
    add_executable(${i}_test ${i}.cpp)
    target_link_libraries(${i}_test
                        synapse ${Boost_LIBRARIES})
    add_serial_test(${i}_test)
endforeach()
