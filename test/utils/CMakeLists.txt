add_executable(test_storage test_storage.cpp)
target_link_libraries(test_storage coreneuron10_common storage  ${Boost_LIBRARIES})

add_executable(test_argv_data test_argv_data.cpp)
target_link_libraries(test_argv_data ${Boost_LIBRARIES})

foreach(test_name storage argv_data)
    if(SLURM_FOUND)
	add_test(NAME "${test_name}" COMMAND ${SLURM_SRUN_COMMAND} --time=00:00:10 "test_${test_name}")
    else()
	add_test(NAME "${test_name}" COMMAND "test_${test_name}")
    endif()
endforeach()
