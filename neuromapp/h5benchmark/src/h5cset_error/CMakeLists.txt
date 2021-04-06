find_package(HDF5 REQUIRED)

add_executable(h5cset_error "${PROJECT_SOURCE_DIR}/h5cset_error.c")
target_link_libraries(h5cset_error ${HDF5_C_LIBRARIES})
