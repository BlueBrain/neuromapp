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


# Ceph - Rados backend
if(IS_DIRECTORY ${CEPH_PATH}/include/rados)
  include_directories(${CEPH_PATH}/include/rados)
  message (STATUS "Using CEPH in root dir: ${CEPH_PATH}")
  set(CEPH_LIBRARIES "ceph_store")
  set(ceph_link_flags "-L${CEPH_PATH}/lib64 -lrados")
  add_definitions(-DCEPH_STORE)

  add_library (ceph_store ceph/ceph_store.cpp)

  set_target_properties(ceph_store PROPERTIES
                  LINK_FLAGS "-lrados")

  install (TARGETS ceph_store DESTINATION lib)
  install (FILES ceph/ceph_store.h DESTINATION include)
else()
  message(STATUS "Couldn't find SKV library, ignoring it.")
  set(CEPH_LIBRARIES "")
  set(ceph_link_flags "")
endif()

add_executable(MPI_Exec_kv utils/statistic.cpp mpiexec.cpp )

# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
target_link_libraries (MPI_Exec_kv map_store ${CEPH_LIBRARIES} ${ceph_link_flags} ${MPI_CXX_LIBRARIES})

set_target_properties(MPI_Exec_kv PROPERTIES
		COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")

install (TARGETS MPI_Exec_kv DESTINATION bin)

