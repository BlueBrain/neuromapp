add_executable(hdf5_read h5read.cpp)
target_link_libraries(hdf5_read h5read ${Boost_LIBRARIES} ${MPI_CXX_LIBRARIES} ${MPI_C_LIBRARIES} ${HDF_LIBRARIES})
add_mpi_test(hdf5_read)