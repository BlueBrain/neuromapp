#exception test
add_executable(exception exception.cpp)
target_link_libraries(exception ${Boost_LIBRARIES})

#driver test
add_executable(application app.cpp ${PROJECT_SOURCE_DIR}/neuromapp/app/driver.cpp)
target_link_libraries(application ${Boost_LIBRARIES})

#list of tests
set(name exception application)

#loop over tests for slurm
foreach(i ${name})
add_serial_test(${i})
endforeach()
