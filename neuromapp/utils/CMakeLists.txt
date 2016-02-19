include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#generate the file for the in put, create in the build directory
configure_file("${PROJECT_SOURCE_DIR}/neuromapp/utils/mpi/mpi_path.h.in"
               "${PROJECT_BINARY_DIR}/neuromapp/utils/mpi/mpi_path.h")

add_library (storage
             storage/neuromapp_data.cpp
             storage/storage.cpp )
