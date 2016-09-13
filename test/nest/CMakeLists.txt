include_directories(${HDF5_INCLUDE_DIRS})


#list of tests
set(nest_tests synapses pool)

#loop over nest test for creation
foreach(i ${nest_tests})
    add_executable(${i} ${i}.cpp)
    target_link_libraries(${i} nest_synapse)#${Boost_LIBRARIES})
    add_serial_test(${i})
endforeach()

add_executable(nestdistri nestdistri.cpp)
target_link_libraries(nestdistri
                      nest_event_passing
                      nest_environment
                      ${Boost_LIBRARIES}
                      ${MPI_CXX_LIBRARIES}
                      ${MPI_C_LIBRARIES})
add_mpi_test(nestdistri)

add_executable(h5import_read h5import.cpp)
target_link_libraries(h5import_read
                            h5import
                            coreneuron10_environment
                            ${Boost_LIBRARIES}
                            ${MPI_CXX_LIBRARIES}
                            ${MPI_C_LIBRARIES}
                            ${HDF_LIBRARIES})
add_mpi_test(h5import_read)
