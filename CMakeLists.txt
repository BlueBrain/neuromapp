cmake_minimum_required (VERSION 2.8.10)
project (neuromapp)

# The version number.
set(LAST_RELEASE 1.0.0)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION_ABI "1")


list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
include(Compiler)
string(REPLACE "-std=c++11" " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

set(Boost_USE_STATIC_LIBS  ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_DIALECT_OPT_C99}")


#find_package(Boost COMPONENTS thread date_time program_options filesystem system REQUIRED)
find_package(Boost 1.44 REQUIRED program_options unit_test_framework system filesystem)
find_package(SLURM)
#boost include
include_directories(${Boost_INCLUDE_DIRS})
#for files that are generated automatically
include_directories(${PROJECT_BINARY_DIR}/neuromapp)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g ")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

enable_testing()

include_directories(neuromapp)
add_subdirectory (neuromapp)

#prepare the input
execute_process(COMMAND unzip -o ${PROJECT_SOURCE_DIR}/neuromapp/coreneuron_1.0/common/data/bench.101392.zip
                              -d ${PROJECT_BINARY_DIR}/neuromapp/coreneuron_1.0/test/bench.101392)

#prepare the ref solution for the test
execute_process(COMMAND tar -xzvf ${PROJECT_SOURCE_DIR}/neuromapp/coreneuron_1.0/common/data/rhs_d_ref.tar.gz
                            -C    ${PROJECT_BINARY_DIR}/neuromapp/coreneuron_1.0/test)
