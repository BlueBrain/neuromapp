add_executable(hdf5_distri_columns distributed_driver.cpp H5SynapseLoader_columns.cpp )

target_link_libraries (hdf5_distri_columns
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES}
                       ${HDF5_LIBRARIES})

install (TARGETS hdf5_distri_columns DESTINATION bin)
