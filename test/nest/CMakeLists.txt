#list of tests
set(nest_tests synapses pool)

#loop over nest test for creation
foreach(i ${nest_tests})
    add_executable(${i} ${i}.cpp)
    target_link_libraries(${i} synapse ${Boost_LIBRARIES})
    add_serial_test(${i})
endforeach()

add_executable(distributed distributed.cpp)
target_link_libraries(distributed nest_dist ${Boost_LIBRARIES} ${MPI_CXX_LIBRARIES} ${MPI_C_LIBRARIES})
add_mpi_test(distributed)