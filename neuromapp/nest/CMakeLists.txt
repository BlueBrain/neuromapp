add_subdirectory (synapse)


add_library (nest_dist eventdelivermanager.cpp simulationmanager.cpp simulationmanager.cpp mpi_manager.cpp)

install (TARGETS nest_dist DESTINATION lib)
install (FILES eventdelivermanager.h
               simulationmanager.h
               simulationmanager.h
               mpi_manager.h DESTINATION include)

               
target_link_libraries (nest_dist
                       nest_manager
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES})
                       

add_executable(nest_dist_exec distributed_driver.cpp )

target_link_libraries (nest_dist_exec
                       nest_dist
                       nest_manager
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES})

install (TARGETS nest_dist_exec DESTINATION bin)
