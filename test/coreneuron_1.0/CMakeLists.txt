install (FILES test_header.hpp DESTINATION include)

#list of tests
set(tests kernel solver cstep queueing)

#loop over tests for creation
foreach(i ${tests})
    add_executable(${i}test ${i}.cpp)
    target_link_libraries(${i}test coreneuron10_common
                                   coreneuron10_cstep
								   coreneuron10_queueing
                                   coreneuron10_kernel
                                   coreneuron10_solver
                                   storage ${Boost_LIBRARIES})
    if(SLURM_FOUND)
        add_test(NAME ${i}test COMMAND ${SLURM_SRUN_COMMAND} --time=00:00:10 ${i}test)
    else()
        add_test(${i} ${i}test)
    endif()
endforeach()
