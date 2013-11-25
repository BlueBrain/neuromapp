cmake_minimum_required (VERSION 2.6)

########################################################################
#
# Debug or Release 
#
########################################################################

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -DNDEBUG -O3 -mavx  " CACHE STRING "Compiler flags for Release compiles.")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -O0 -m64 -mavx " CACHE STRING "Compiler flags for Debug compiles.")

########################################################################
#
# Project and version information
#
########################################################################

project (miniapps) 
set (MINI_APPS_VERSION_MAJOR 0)
set (MINI_APPS_VERSION_MINOR 1)
	
########################################################################
#
# Options
#
########################################################################

# normal/cyme
option (ORIGINAL_VERSION "compilation original version" ON)
option (CYME_VERSION "compilation cyme version" ON)

# cyme
set(SIMD_TECH sse CACHE STRING "SIMD technology: sse, avx, qpx and mic")
option (CYME_FMA "support FMA, very experiemental the parser is not still ready, it can not compile" OFF)
option (CYME_MATH "support lmass lmass_simd from IBM" OFF)

#test 
option (MINIAPPS_TESTS "Build the regression tests" OFF)

########################################################################
#
# Find BOOST
#
########################################################################

#this depends of the associated module
#set(Boost_NO_SYSTEM_PATHS TRUE)
set(BOOST_DIR $ENV{BOOST_DIR} CACHE PATH "Path to the Boost installation (or to the Boost source)")
set(BOOST_ROOT ${BOOST_DIR})
#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
find_package (Boost  COMPONENTS unit_test_framework system chrono)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Boost libraries not found. Please specify location using the BOOST_DIR variable")
endif()


########################################################################
#
# main following user choice
#
########################################################################

if(ORIGINAL_VERSION)
    add_subdirectory ("${PROJECT_SOURCE_DIR}/original_version/main")
endif(ORIGINAL_VERSION)

if(CYME_VERSION)
    add_subdirectory ("${PROJECT_SOURCE_DIR}/cyme_version/main")
endif(CYME_VERSION)

########################################################################
#
# Find SIMD
#
########################################################################

if(CYME_FMA)
   add_definitions(-mfma -mavx -D__FMA__)
endif(CYME_FMA)


########################################################################
#
# math library 
#
########################################################################
if(CYME_MATH)
find_library(MASS_LIBRARY
             NAMES mass
             PATHS /opt/ibmcmp/xlmass/bg/7.3/bglib64/)

find_library(SIMD_MASS_LIBRARY
             NAMES mass_simd
             PATHS /opt/ibmcmp/xlmass/bg/7.3/bglib64/)
endif(CYME_MATH)

########################################################################
#
# test 
#
########################################################################


if(MINIAPPS_TESTS)
    include_directories(${PROJECT_SOURCE_DIR})
    include(CTest)
    enable_testing()
    set(BUILDNAME "${PROJECT_VERSION}" CACHE STRING "Name of build on the dashboard")
      mark_as_advanced(BUILDNAME)
    add_subdirectory (regression)
endif()



