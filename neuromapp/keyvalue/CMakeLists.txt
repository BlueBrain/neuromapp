set(MPI_STATIC ON)
find_package(MPI REQUIRED )

include_directories(${MPI_INCLUDE_PATH})

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

# Add keyvalue to the mini-app library
add_library (keyvalue main.cpp)

install (TARGETS keyvalue DESTINATION lib)
install (FILES keyvalue.h DESTINATION include)

# STL MAP backend
add_library (map_store map/map_store.cpp)

install (TARGETS map_store DESTINATION lib)
install (FILES map/map_store.h DESTINATION include)

# SKV IBM backend
# Use 'export LIB=SKV_PATH/cnk/lib/:$LIB' to help cmake find the SKV path
find_library( SKV NAMES skv_client_mpi skv_common it_api fxlogger )
#set(SKV_PATH "/gpfs/bbp.cscs.ch/home/jplanas/bgas/skv/cnk/" CACHE PATH "IBM SKV library root directory path")
if(SKV)
  # Get SKV lib path
  get_filename_component(SKV_LIB_PATH "${SKV}" PATH)
  # Get SKV root path
  get_filename_component(SKV_PATH "${SKV_LIB_PATH}" PATH)
  message(STATUS "Found SKV root directory: ${SKV_PATH}")
  include_directories(${SKV_PATH}/include)
  set(SKV_LIBRARIES "skv_store")
  set(SKV_LINK_FLAGS "-L ${SKV_PATH}/lib -lskv_client_mpi -lskv_common -lit_api -lfxlogger")
  add_definitions(-DSKV_STORE)

  add_library (skv_store skv/skv_store.cpp)
  set_target_properties(skv_store PROPERTIES
                  LINK_FLAGS "${SKV_LINK_FLAGS}")
  set_target_properties(skv_store PROPERTIES
                  COMPILE_FLAGS "-I ${SKV_PATH}/include -I ${SKV_PATH}/include/skv")

  install (TARGETS skv_store DESTINATION lib)
  install (FILES skv/skv_store.h DESTINATION include)
else()
  message(STATUS "Couldn't find SKV library, ignoring it.")
  set(SKV_LIBRARIES "")
  set(SKV_LINK_FLAGS "")
endif()



# Ceph - Rados backend
# Use 'export LIB=CEPH_PATH/lib64/:$LIB' to help cmake find the Ceph path
find_library( CEPH NAMES rados )
#if(IS_DIRECTORY ${CEPH_PATH}/include/rados)
if(CEPH)
  # Get Ceph lib path
  get_filename_component(CEPH_LIB_PATH "${CEPH}" PATH)
  # Get Ceph root path
  get_filename_component(CEPH_PATH "${CEPH_LIB_PATH}" PATH)
  message(STATUS "Found Ceph root directory: ${CEPH_PATH}")
  include_directories(${CEPH_PATH}/include/rados)
  set(CEPH_LIBRARIES "ceph_store")
  set(CEPH_LINK_FLAGS "-L${CEPH_PATH}/lib64 -lrados")
  add_definitions(-DCEPH_STORE)

  add_library (ceph_store ceph/ceph_store.cpp)
  set_target_properties(ceph_store PROPERTIES
                  LINK_FLAGS "-lrados")

  install (TARGETS ceph_store DESTINATION lib)
  install (FILES ceph/ceph_store.h DESTINATION include)
else()
  message(STATUS "Couldn't find Ceph library, ignoring it.")
  set(CEPH_LIBRARIES "")
  set(CEPH_LINK_FLAGS "")
endif()

add_executable(MPI_Exec_kv utils/statistic.cpp mpiexec.cpp )

# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q, use MPI_CXX_LIBRARIES as a workaround
set(MPI_LIBRARIES ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY})
# SKV_LINK_FLAGS must go after SKV_LIBRARIES
target_link_libraries (MPI_Exec_kv map_store ${SKV_LIBRARIES} ${SKV_LINK_FLAGS} ${CEPH_LIBRARIES} ${CEPH_LINK_FLAGS} ${MPI_CXX_LIBRARIES})

install (TARGETS MPI_Exec_kv DESTINATION bin)

