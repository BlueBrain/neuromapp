cmake_minimum_required(VERSION 2.8.9)
project(ll_counter)

include(CMake/Compiler.cmake)
set(CMAKE_CXX_FLAGS "${CXX_DIALECT_OPT_CXX11} ${COMPILER_OPT_ARCH_NATIVE} ${CMAKE_CXX_FLAGS}")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 2)

set(LLC_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CMAKE_BUILD_TYPE Release)

add_subdirectory(llc)
add_subdirectory(op_latency)





