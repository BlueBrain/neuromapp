#exception test
add_executable(exceptiontest exception.cpp)
target_link_libraries(exceptiontest ${Boost_LIBRARIES})

#driver test
add_executable(apptest app.cpp ${PROJECT_SOURCE_DIR}/neuromapp/app/driver.cpp)
target_link_libraries(apptest ${Boost_LIBRARIES})

#list of tests
set(tests exception app)

#loop over tests for slurm
foreach(i ${tests})
    if(SLURM_FOUND)
        add_test(NAME ${i}test COMMAND ${SLURM_SRUN_COMMAND} --time=00:00:10 ${i}test)
    else()
        add_test(${i} ${i}test)
    endif()
endforeach()
