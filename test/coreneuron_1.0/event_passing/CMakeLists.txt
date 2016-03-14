#list of tests
set(event_tests environment)

#loop over event_tests for creation
foreach(i ${event_tests})
    add_executable(${i} ${i}.cpp)
    target_link_libraries(${i} coreneuron10_environment
                               ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
                               ${Boost_SYSTEM_LIBRARIES}
                               ${Boost_FILESYSTEM_LIBRARIES})
    add_serial_test(${i})
endforeach()
