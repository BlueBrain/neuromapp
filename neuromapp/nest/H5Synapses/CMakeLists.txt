include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})


add_library (h5synapses H5Synapses.cpp NESTNodeSynapse.cpp nest_kernel.cpp)
target_link_libraries(h5synapses coreneuron10_queueing coreneuron10_environment)
target_include_directories(h5synapses PRIVATE ${HDF5_INCLUDE_DIRS})

install (TARGETS h5synapses DESTINATION lib)
install (FILES H5Synapses.h nest_kernel.h H5SynapseLoader.h kernels.h NESTNodeSynapse.h DESTINATION include)


add_executable(nest_h5synapses distributed_driver.cpp )

target_link_libraries (nest_h5synapses
                       h5synapses
                       coreneuron10_queueing
                       coreneuron10_environment
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES}
                       ${HDF5_LIBRARIES})
target_include_directories(nest_h5synapses PRIVATE ${HDF5_INCLUDE_DIRS})

install (TARGETS nest_h5synapses DESTINATION bin)
