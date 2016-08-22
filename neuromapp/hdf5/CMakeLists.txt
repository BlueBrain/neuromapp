add_executable(hdf5_distri_columns distributed_driver.cpp H5SynapseLoader.cpp )

target_link_libraries (hdf5_distri_columns
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES}
                       ${HDF5_LIBRARIES})

target_include_directories(hdf5_distri_columns PRIVATE ${HDF5_INCLUDE_DIRS})

install (TARGETS hdf5_distri_columns DESTINATION bin)
