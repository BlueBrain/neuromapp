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

#SKV IBM backend
#set(SKV_PATH "/gpfs/bbp.cscs.ch/home/jplanas/bgas/skv/cnk/" CACHE PATH "IBM SKV library root directory path")
add_executable(MPI_Exec_kv utils/statistic.cpp mpiexec.cpp )

# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
target_link_libraries (MPI_Exec_kv map_store ${MPI_CXX_LIBRARIES})

# SKV link flags must be after SKV_LIBRARIES
set(MPI_LIBRARIES ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY})
#target_link_libraries (MPI_Exec_kv mpikey-value map_store ${SKV_LIBRARIES} ${skv_link_flags} ${MPI_LIBRARIES})

# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
target_link_libraries (MPI_Exec_kv map_store ${SKV_LIBRARIES} ${skv_link_flags})

if (MPI_COMPILE_FLAGS)
	set_target_properties(MPI_Exec_kv PROPERTIES
		COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if (MPI_LINK_FLAGS)
	set_target_properties(MPI_Exec_kv PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install (TARGETS MPI_Exec_kv DESTINATION bin)
