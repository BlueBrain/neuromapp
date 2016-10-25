#list of tests
set(serial_event_tests environment queueing)
set(mpi_event_tests spike)

#loop over serial_event_tests for creation
foreach(i ${serial_event_tests})
    add_executable(${i} ${i}.cpp)
    target_link_libraries(${i} coreneuron10_environment
                               coreneuron10_queueing
                               storage
                               coreneuron10_solver
                               coreneuron10_cstep
                               coreneuron10_queue
                               ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
                               ${Boost_SYSTEM_LIBRARIES}
                               ${Boost_FILESYSTEM_LIBRARIES}
                               )
    add_serial_test(${i})
endforeach()

#loop over mpi_event_tests for creation
foreach(i ${mpi_event_tests})
    add_executable(${i} ${i}.cpp)
    target_link_libraries(${i} coreneuron10_environment
                               coreneuron10_queueing
                               storage
                               coreneuron10_solver
                               coreneuron10_cstep
                               coreneuron10_queue
                               ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES}
                               ${Boost_SYSTEM_LIBRARIES}
                               ${Boost_FILESYSTEM_LIBRARIES}
                               ${MPI_CXX_LIBRARIES}
                               ${MPI_C_LIBRARIES}
                               )
    add_mpi_test(${i})
endforeach()
