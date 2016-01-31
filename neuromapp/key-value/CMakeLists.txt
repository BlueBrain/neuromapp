set(MPI_STATIC ON)
find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#Add key-value to the mini-app library
add_library (key-value main.cpp)
target_link_libraries (key-value  ${MPI_LIBRARIES})

install (TARGETS key-value DESTINATION lib)
install (FILES key-value.h DESTINATION include)

#Key-value interface header file
install (FILES kv-iface.h DESTINATION include)


#STL MAP backend
add_library (map_store map/map_store.cpp)

install (TARGETS map_store DESTINATION lib)
install (FILES map/map_store.h DESTINATION include)

add_executable(MPI_Exec_kv mpiexec.cpp)


# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
target_link_libraries (MPI_Exec_kv map_store ${MPI_LIBRARIES}) 


