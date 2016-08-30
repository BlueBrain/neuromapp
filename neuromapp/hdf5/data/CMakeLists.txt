include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})


#generate the file for the in put, create in the build directory
configure_file("${PROJECT_SOURCE_DIR}/neuromapp/hdf5/data/path.h.in"
               "${PROJECT_BINARY_DIR}/neuromapp/hdf5/data/path.h")
               
