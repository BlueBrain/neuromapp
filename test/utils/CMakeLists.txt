add_executable(store storage.cpp)
target_link_libraries(store coreneuron10_common storage  ${Boost_LIBRARIES})

add_executable(argv_data argv_data.cpp)
target_link_libraries(argv_data ${Boost_LIBRARIES})

foreach(name store argv_data)
    add_serial_test(${name})
endforeach()

add_executable(timer timer.cpp)
target_link_libraries(timer ${Boost_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_mpi_test(timer)
