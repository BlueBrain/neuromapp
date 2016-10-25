include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})


add_subdirectory(drivers)
add_subdirectory(data)

add_library(h5read h5reader.cpp )

target_link_libraries (h5read
                       ${MPI_CXX_LIBRARIES}
                       ${MPI_C_LIBRARIES}
                       ${Boost_LIBRARIES}
                       ${HDF5_LIBRARIES})

target_include_directories(h5read PRIVATE ${HDF5_INCLUDE_DIRS})

install (TARGETS h5read DESTINATION lib)
