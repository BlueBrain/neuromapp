
include_directories(${MPI_INCLUDE_PATH})
message("----------------------" ${MPI_INCLUDE_PATH})
add_executable(argtest  test.cpp)
target_link_libraries(argtest  ${Boost_LIBRARIES} ${MPI_LIBRARIES})

if(SLURM_FOUND)
add_test(NAME argtest COMMAND ${SLURM_SRUN_COMMAND} --time=00:00:10 argtest)
else()
add_test(arg argtest)
endif()
