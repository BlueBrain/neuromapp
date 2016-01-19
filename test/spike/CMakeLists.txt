add_executable(spiketest test.cpp)
target_link_libraries(spiketest spike ${Boost_LIBRARIES})

if(SLURM_FOUND)
add_test(NAME spiketest COMMAND ${SLURM_SRUN_COMMAND} --time=00:00:10 spiketest)
else()
add_test(spike spiketest)
endif()
