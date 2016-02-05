add_executable(args args.cpp)
target_link_libraries(args  ${Boost_LIBRARIES})
add_serial_test(args)

add_executable(statistic_mpi statistic.cpp ${PROJECT_SOURCE_DIR}/neuromapp/keyvalue/utils/statistic.cpp)
target_link_libraries(statistic_mpi  ${Boost_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_mpi_test(statistic_mpi)