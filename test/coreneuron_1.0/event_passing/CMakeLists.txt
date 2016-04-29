#list of tests
set(event_tests environment queueing )#spike)

#loop over event_tests for creation
foreach(i ${event_tests})
    add_executable(${i} ${i}.cpp)
    target_link_libraries(${i} coreneuron10_environment
                               coreneuron10_queueing
                               storage
                               coreneuron10_solver
                               coreneuron10_cstep
                               ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
                               ${Boost_SYSTEM_LIBRARIES}
                               ${Boost_FILESYSTEM_LIBRARIES}
                               ${MPI_CXX_LIBRARIES}
                               ${MPI_C_LIBRARIES})
    add_serial_test(${i})
endforeach()
