set(MPI_STATIC ON)
find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#Add iobench to the mini-app library
add_library (iobench main.cpp)

install (TARGETS iobench DESTINATION lib)
install (FILES iobench.h DESTINATION include)

# MPI+OMP executable
#add_executable(MPI_Exec_io utils/statistics.cpp utils/tools.cpp iobench.cpp )
add_executable(MPI_Exec_io iobench.cpp )
set_target_properties(MPI_Exec_io PROPERTIES COMPILE_FLAGS "-DIO_MPI")

# OMP executable
#add_executable(iobench-omp utils/statistics.cpp utils/tools.cpp iobench.cpp )
add_executable(iobench-omp iobench.cpp )
target_link_libraries (iobench-omp ${Boost_PROGRAM_OPTIONS_LIBRARIES})

# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
target_link_libraries (MPI_Exec_io ${MPI_CXX_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARIES})

set_target_properties(MPI_Exec_io PROPERTIES
		COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -DIO_MPI")


install (TARGETS MPI_Exec_io DESTINATION bin)
install (TARGETS iobench-omp DESTINATION bin)
