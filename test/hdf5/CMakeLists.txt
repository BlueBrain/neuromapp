
add_executable(h5nest h5nest_columns.cpp)
target_link_libraries(h5nest h5import ${Boost_LIBRARIES} ${MPI_CXX_LIBRARIES} ${MPI_C_LIBRARIES} ${HDF_LIBRARIES})
add_mpi_test(h5nest)