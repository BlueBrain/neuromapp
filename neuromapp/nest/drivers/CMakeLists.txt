

add_executable(nest_h5import_distributed_exec nest_event_passing_distributed.cpp )

target_link_libraries (nest_event_passing_dirstibuted_exec
                       nestkernel_event_passing
                       nestkernel_environment
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES})

install (TARGETS nest_event_passing_dirstibuted_exec DESTINATION bin)




add_executable(nest_event_passing_distributed_exec nest_event_passing_dirstibuted.cpp )

target_link_libraries (nest_event_passing_distributed_exec
                       nestkernel_event_passing
                       nestkernel_environment
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES})

install (TARGETS nest_event_passing_distributed_exec DESTINATION bin)
