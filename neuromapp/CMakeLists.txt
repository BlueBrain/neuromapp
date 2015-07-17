cmake_minimum_required (VERSION 2.8.11)
project (neuromapp)

#set( CMAKE_C_FLAGS  ${CMAKE_C_FLAGS} "-std=c99")
#find_package(Boost COMPONENTS thread date_time program_options filesystem system REQUIRED)
set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost 1.44 REQUIRED program_options unit_test_framework system)
include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

enable_testing()

include_directories(neuromapp)
add_subdirectory (neuromapp)

