#list of tests
set(tests kernel solver cstep queue)

#loop over tests for creation
foreach(i ${tests})
    add_executable(${i} ${i}.cpp)
    target_link_libraries(${i} coreneuron10_common
                               coreneuron10_cstep
                               coreneuron10_kernel
                               coreneuron10_solver
                               coreneuron10_queue
                               storage
                               ${Boost_LIBRARIES})
    add_serial_test(${i})
endforeach()

add_subdirectory (event_passing)
