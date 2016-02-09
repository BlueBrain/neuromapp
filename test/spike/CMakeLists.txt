
add_executable(spiketest test.cpp)
target_link_libraries(spiketest ${MPI_CXX_LIBRARIES} distributedgraph ${Boost_LIBRARIES})
add_mpi_test(spiketest)

