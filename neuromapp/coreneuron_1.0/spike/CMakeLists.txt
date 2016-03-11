include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#Add spike to the mini-app library
add_library (spike main.cpp)

install (TARGETS spike DESTINATION lib)
install (FILES spike.h DESTINATION include)

#Add mpispike to the mini-app library
add_library (spike_exchange environment.cpp)

install (TARGETS spike_exchange DESTINATION lib)
install (FILES environment.h algos.hpp DESTINATION include)
target_link_libraries (spike_exchange coreneuron10_queueing)

#Add an MPI executable
add_executable(spike_exec mpi_spike_exec.cpp)
target_link_libraries (spike_exec spike_exchange ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})

install (TARGETS spike_exec DESTINATION bin)
