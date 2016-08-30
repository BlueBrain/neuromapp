include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})


add_library(nest_h5import_driver h5import.cpp )
target_link_libraries( nest_h5import_driver
                       ${Boost_LIBRARIES} )
install (TARGETS nest_h5import_driver DESTINATION lib)

add_executable(nest_h5import_distributed_exec h5import_distributed.cpp )
target_link_libraries (nest_h5import_distributed_exec
                       h5import
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES}
                       ${HDF5_LIBRARIES})

install (TARGETS nest_h5import_distributed_exec DESTINATION bin)


add_library(nest_event_passing_driver event_passing.cpp )
target_link_libraries( nest_event_passing_driver
                       ${Boost_LIBRARIES} )
install (TARGETS nest_event_passing_driver DESTINATION lib)

add_executable(nest_event_passing_distributed_exec event_passing_distributed.cpp )

target_link_libraries (nest_event_passing_distributed_exec
                       nest_event_passing
                       nest_environment
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES})

install (TARGETS nest_event_passing_distributed_exec DESTINATION bin)


add_library(nest_synapse synapses.cpp )
target_link_libraries( nest_synapse
                       nest_environment
                       coreneuron10_environment
                       ${Boost_LIBRARIES} )
 install (TARGETS nest_synapse DESTINATION lib)