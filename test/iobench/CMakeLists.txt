add_executable(io-args-omp args.cpp)
target_link_libraries(io-args-omp ${Boost_LIBRARIES})
add_serial_test(io-args-omp)

add_executable(io-args-mpi args.cpp)
target_link_libraries(io-args-mpi ${Boost_LIBRARIES} ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_mpi_test(io-args-mpi)

add_executable(io-stats-omp stats.cpp)
target_link_libraries(io-stats-omp ${Boost_LIBRARIES})
add_serial_test(io-stats-omp)

add_executable(io-stats-mpi stats.cpp)
target_link_libraries(io-stats-mpi ${Boost_LIBRARIES} ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
set_target_properties(io-stats-mpi PROPERTIES
        COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -DIO_MPI")
add_mpi_test(io-stats-mpi)

add_executable(io-bencht-omp bench.cpp ${PROJECT_SOURCE_DIR}/neuromapp/iobench/benchmark.cpp)
target_link_libraries(io-bencht-omp ${Boost_LIBRARIES})
add_serial_test(io-bencht-omp)

add_executable(io-bencht-mpi bench.cpp ${PROJECT_SOURCE_DIR}/neuromapp/iobench/benchmark.cpp)
target_link_libraries(io-bencht-mpi ${Boost_LIBRARIES} ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_mpi_test(io-bencht-mpi)

