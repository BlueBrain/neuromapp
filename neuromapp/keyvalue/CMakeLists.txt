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
set(MAP_MAPP_LIBRARY "map_store")

install (TARGETS map_store DESTINATION lib)
install (FILES map/map_store.h DESTINATION include)

# SKV IBM backend
find_package(Skv)
if (${SKV_FOUND})
    include_directories(${SKV_INCLUDE_DIRS})
    add_definitions(-DSKV_STORE)

    add_library(skv_store skv/skv_store.cpp)
    set_target_properties(skv_store PROPERTIES
                      LINK_FLAGS "${SKV_LIBRARIES}")

    install (TARGETS skv_store DESTINATION lib)
    install (FILES skv/skv_store.h DESTINATION include)

    set(SKV_MAPP_LIBRARY "skv_store")
else()
    set(SKV_MAPP_LIBRARY "")
    set(SKV_LIBRARIES "")
endif()


# CEPH - Rados backend
find_package(Ceph)
if (${CEPH_FOUND})
    include_directories(${CEPH_INCLUDE_DIRS})
    add_definitions(-DCEPH_STORE)

    add_library(ceph_store ceph/ceph_store.cpp)
    set_target_properties(ceph_store PROPERTIES
                      LINK_FLAGS "${CEPH_LIBRARIES}")

    install (TARGETS ceph_store DESTINATION lib)
    install (FILES ceph/ceph_store.h DESTINATION include)

    set(CEPH_MAPP_LIBRARY "ceph_store")
else()
    set(CEPH_MAPP_LIBRARY "")
    set(CEPH_LIBRARIES "")
endif()

add_executable(MPI_Exec_kv utils/statistic.cpp mpiexec.cpp )

# Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q, use MPI_CXX_LIBRARIES as a workaround
set(MPI_LIBRARIES ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY})
# SKV_LINK_FLAGS must go after SKV_LIBRARIES
target_link_libraries (MPI_Exec_kv ${MAP_MAPP_LIBRARY} ${SKV_MAPP_LIBRARY} ${SKV_LIBRARIES} ${CEPH_MAPP_LIBRARY} ${CEPH_LIBRARIES} ${MPI_CXX_LIBRARIES})

install (TARGETS MPI_Exec_kv DESTINATION bin)

