#list of tests
set(tests kernel solver cstep queueing)

#loop over tests for creation
foreach(i ${tests})
    add_executable(${i} ${i}.cpp)
    target_link_libraries(${i} coreneuron10_common
                               coreneuron10_cstep
                               coreneuron10_queueing
                               coreneuron10_kernel
                               coreneuron10_solver
                               storage ${Boost_LIBRARIES})
    add_serial_test(${i})
endforeach()
