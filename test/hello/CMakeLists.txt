add_executable(hellotest test.cpp)
target_link_libraries(hellotest hello ${Boost_LIBRARIES})

if(SLURM_FOUND)
add_test(NAME hellotest COMMAND ${SLURM_SRUN_COMMAND} --time=00:00:10 hellotest)
else()
add_test(hello hellotest)
endif()
