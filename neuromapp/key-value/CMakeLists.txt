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

#SKV IBM backend
#set(SKV_PATH "/gpfs/bbp.cscs.ch/home/jplanas/bgas/skv/cnk/" CACHE PATH "IBM SKV library root directory path")
add_executable(MPI_Exec_kv mpiexec.cpp)


# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
target_link_libraries (MPI_Exec_kv map_store ${MPI_LIBRARIES}) 


if(IS_DIRECTORY ${SKV_PATH})
  include_directories(${SKV_PATH}/include)
  message (STATUS "Using SKV in root dir: ${SKV_PATH}")
  set(SKV_LIBRARIES "skv_store")
  set(skv_link_flags "-L ${SKV_PATH}/lib -lskv_client_mpi -lskv_common -lit_api -lfxlogger")
  add_definitions(-DSKV_STORE)


  add_library (skv_store skv/skv_store.cpp)

  set_target_properties(skv_store PROPERTIES
                  LINK_FLAGS "-L ${SKV_PATH}/lib -lskv_client_mpi -lskv_common -lit_api -lfxlogger")


  set_target_properties(skv_store PROPERTIES
                  COMPILE_FLAGS "-I ${SKV_PATH}/include -I ${SKV_PATH}/include/skv")

  install (TARGETS skv_store DESTINATION lib)
  install (FILES skv/skv_store.h DESTINATION include)

else()
  message(STATUS "Couldn't find SKV library, ignoring it.")
  set(SKV_LIBRARIES "")
  set(skv_link_flags "")
endif()

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
