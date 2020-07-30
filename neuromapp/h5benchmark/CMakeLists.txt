cmake_minimum_required(VERSION 3.2)
project(H5BENCHMARK VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(EXTLIB_FROM_SUBMODULES ON)

find_package(HDF5 REQUIRED)
find_package(MorphIO REQUIRED PATHS ./morphio/build)
find_package(MorphoKit REQUIRED PATHS ./morpho-kit/build)
find_package(Boost REQUIRED COMPONENTS filesystem system)

add_subdirectory(src)

add_executable(h5benchmark ./src/h5benchmark.cpp)
target_link_libraries(h5benchmark hdf5::hdf5)
target_link_libraries(h5benchmark MorphIO::morphio)
target_link_libraries(h5benchmark MorphoKit::morphokit)
target_link_libraries(h5benchmark Boost::filesystem Boost::system})
