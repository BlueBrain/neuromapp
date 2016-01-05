find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#Add spike to the mini-app library
add_library (spike main.cpp)

install (TARGETS spike DESTINATION lib)
install (FILES spike.h DESTINATION include)

#Add mpispike to the mini-app library
add_library (mpispike mpispike.cpp)

install (TARGETS mpispike DESTINATION lib)
install (FILES mpispike.h DESTINATION include)

#Add an MPI executable
add_executable(MPI_Exec mpiexec.cpp)
target_link_libraries (MPI_Exec mpispike ${MPI_LIBRARIES})

if (MPI_COMPILE_FLAGS)
	set_target_properties(MPI_Exec PROPERTIES
		COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if (MPI_LINK_FLAGS)
	set_target_properties(MPI_Exec PROPERTIES
		LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install (TARGETS MPI_Exec DESTINATION bin)
