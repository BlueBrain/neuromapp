add_executable(spiketest test.cpp)
target_link_libraries(spiketest
                      spike_exchange
                      spike
                      coreneuron10_queueing
                      ${Boost_LIBRARIES}
		      ${MPI_CXX_LIBRARIES}
		      ${MPI_C_LIBRARIES})
add_mpi_test(spiketest)
