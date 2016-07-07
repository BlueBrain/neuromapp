add_subdirectory (synapse)


add_executable(nest_dist_exec distributed_driver.cpp eventdelivermanager.cpp simulationmanager.cpp)
target_link_libraries (nest_dist_exec
                       nest_manager
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES})

install (TARGETS nest_dist_exec DESTINATION bin)
