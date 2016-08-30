include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

add_library( h5read_driver h5read.cpp )
target_link_libraries( h5read_driver
                       
                       ${Boost_LIBRARIES})
install (TARGETS h5read_driver DESTINATION lib)

add_executable(h5read_distributed_exec h5read_distributed.cpp )

target_link_libraries (h5read_distributed_exec
                       h5read
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES}
                       ${HDF5_LIBRARIES})
                       
set_target_properties(h5read_distributed_exec PROPERTIES
        COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -DIO_MPI")

target_include_directories(h5read_distributed_exec PRIVATE ${HDF5_INCLUDE_DIRS})

install (TARGETS h5read_distributed_exec DESTINATION bin)
