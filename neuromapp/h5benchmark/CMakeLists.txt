cmake_minimum_required(VERSION 3.2)
project(H5BENCHMARK VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
option(INTERNAL_PACKAGES "Use packages from submodules" ON)

if (INTERNAL_PACKAGES)
    list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/morphio/build")
    list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/morpho-kit/build")
endif()

find_package(MPI REQUIRED)
find_package(HDF5 REQUIRED)
find_package(MorphoKit REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

set(SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/H5Benchmark.cpp"
                 "${PROJECT_SOURCE_DIR}/src/H5Parser.cpp"
                 "${PROJECT_SOURCE_DIR}/src/H5Util.cpp"
                 "${PROJECT_SOURCE_DIR}/src/IOApi_HDF5.cpp"
                 "${PROJECT_SOURCE_DIR}/src/IOApi_Hi5.cpp"
                 "${PROJECT_SOURCE_DIR}/src/IOApi_MKit.cpp")

add_executable(h5benchmark ${SOURCE_FILES})
add_definitions(-DH5_USE_BOOST)
if (INTERNAL_PACKAGES)
    target_include_directories(h5benchmark SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/morpho-kit/build/include")
endif()
target_link_libraries(h5benchmark ${HDF5_C_LIBRARIES} MorphoKit::morphokit_shared)

install(TARGETS h5benchmark
        RUNTIME DESTINATION bin)
