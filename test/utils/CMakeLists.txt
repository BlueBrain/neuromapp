add_executable(storagetest test.cpp)
target_link_libraries(storagetest coreneuron10_common storage  ${Boost_LIBRARIES})

if(SLURM_FOUND)
add_test(NAME storagetest COMMAND ${SLURM_SRUN_COMMAND} --time=00:00:10 storagetest)
else()
add_test(storage storagetest)
endif()
