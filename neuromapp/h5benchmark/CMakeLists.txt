cmake_minimum_required(VERSION 3.2)
project(H5BENCHMARK VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/morphio/build")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/morpho-kit/build")

find_package(MPI REQUIRED)
find_package(HDF5 REQUIRED)
find_package(MorphoKit REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

set(SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/H5Benchmark.cpp"
                 "${CMAKE_SOURCE_DIR}/src/H5Parser.cpp"
                 "${CMAKE_SOURCE_DIR}/src/H5Util.cpp"
                 "${CMAKE_SOURCE_DIR}/src/IOApi_HDF5.cpp"
                 "${CMAKE_SOURCE_DIR}/src/IOApi_Hi5.cpp"
                 "${CMAKE_SOURCE_DIR}/src/IOApi_MKit.cpp")

add_executable(h5benchmark ${SOURCE_FILES})
target_include_directories(h5benchmark SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/morphio/build/include")
target_include_directories(h5benchmark SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/morpho-kit/build/include")
target_link_libraries(h5benchmark ${HDF5_C_LIBRARIES} MorphoKit::morphokit_shared)
