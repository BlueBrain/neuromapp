set(MPI_STATIC ON)
find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#Add replib to the mini-app library
add_library (replib main.cpp)

install (TARGETS replib DESTINATION lib)
install (FILES replib.h DESTINATION include)

add_executable(MPI_Exec_rl utils/statistics.cpp utils/tools.cpp mpiexec.cpp )

# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
target_link_libraries (MPI_Exec_rl ${MPI_CXX_LIBRARIES})

set_target_properties(MPI_Exec_rl PROPERTIES
		COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")


install (TARGETS MPI_Exec_rl DESTINATION bin)
