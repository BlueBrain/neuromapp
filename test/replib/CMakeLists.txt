add_executable(rl-config config.cpp)
target_link_libraries(rl-config ${Boost_LIBRARIES} ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_mpi_test(rl-config)

add_executable(rl-stats stats.cpp)
target_link_libraries(rl-stats replib-stats ${Boost_LIBRARIES} ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_mpi_test(rl-stats)

add_executable(rl-bench bench.cpp ${PROJECT_SOURCE_DIR}/neuromapp/iobench/benchmark.cpp)
target_link_libraries(rl-bench replib-stats ${Boost_LIBRARIES} ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_mpi_test(rl-bench)
