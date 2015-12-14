add_executable(queueingtest test.cpp)
target_link_libraries(queueingtest queueing ${Boost_LIBRARIES})

if(SLURM_FOUND)
add_test(NAME queueingtest COMMAND ${SLURM_SRUN_COMMAND} --time=00:00:10 queueingtest)
else()
add_test(queueing queueingtest)
endif()
