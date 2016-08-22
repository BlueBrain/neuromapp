set(MPI_STATIC ON)
find_package(MPI REQUIRED )

include_directories(${MPI_INCLUDE_PATH})

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#Add keyvalue to the mini-app library
add_library (keyvalue main.cpp)
#target_link_libraries (keyvalue  ${MPI_LIBRARIES})

install (TARGETS keyvalue DESTINATION lib)
install (FILES keyvalue.h DESTINATION include)

#STL MAP backend
add_library (map_store map/map_store.cpp)

install (TARGETS map_store DESTINATION lib)
install (FILES map/map_store.h DESTINATION include)

add_executable(MPI_Exec_kv utils/statistic.cpp mpiexec.cpp )

# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
target_link_libraries (MPI_Exec_kv map_store ${MPI_CXX_LIBRARIES})

set_target_properties(MPI_Exec_kv PROPERTIES
		COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")


#install (TARGETS MPI_Exec_kv DESTINATION bin)
