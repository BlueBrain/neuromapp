add_executable(spiketest test.cpp)
target_link_libraries(spiketest
                      spike_exchange
                      spike
                      coreneuron10_queueing
                      ${Boost_LIBRARIES})
add_mpi_test(spiketest)
