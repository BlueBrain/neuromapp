#set(MPI_STATIC ON)
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

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
add_executable(MPI_Exec mpiexec.cpp)
target_link_libraries (MPI_Exec spike_exchange ${MPI_CXX_LIBRARIES})

if (MPI_COMPILE_FLAGS)
	set_target_properties(MPI_Exec PROPERTIES
		COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if (MPI_LINK_FLAGS)
	set_target_properties(MPI_Exec PROPERTIES
		LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install (TARGETS MPI_Exec DESTINATION bin)
