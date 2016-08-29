cmake_minimum_required (VERSION 2.8.10)
project (neuromapp)


option(NEUROMAPP_BUILD_RPATH "Export RPATH " OFF)

#hack untill the end of xlc bg/q
if(NEUROMAPP_BUILD_RPATH)
# rpath for nix
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif(NEUROMAPP_BUILD_RPATH)

# The version number.
set(LAST_RELEASE 1.0.0)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION_ABI "1")

option (NEUROMAPP_CURSOR "Allow the use of cursors during input" OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
include(Compiler)
string(REPLACE "-std=c++11" " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_DIALECT_OPT_C99EXT}")


include(BlueGenePortability)

#find_package(Boost COMPONENTS thread date_time program_options filesystem system REQUIRED)
find_package(Boost 1.44 REQUIRED chrono program_options unit_test_framework system filesystem atomic random)
find_package(SLURM)
find_package(MPI)

## MPI link flags need to be overwritten on BG/Q
include(BlueGenePortability)


# include all directories
include_directories(${Boost_INCLUDE_DIRS} ${MPI_C_INCLUDE_PATH} )

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g ")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if(NEUROMAPP_CURSOR)
    MESSAGE(STATUS "Using Cursors")
    add_definitions(-DNEUROMAPP_CURSOR)

    set(CURSES_NEED_NCURSES TRUE)
    find_package(Curses REQUIRED)
    find_package(Readline REQUIRED)

endif(NEUROMAPP_CURSOR)


enable_testing()

include_directories(neuromapp)
add_subdirectory (neuromapp)
#test
include(Testing)
add_subdirectory (test)

#prepare the input
execute_process(COMMAND unzip -o ${PROJECT_SOURCE_DIR}/neuromapp/coreneuron_1.0/common/data/bench.101392.zip
                              -d ${PROJECT_BINARY_DIR}/test/bench.101392)

#prepare the ref solution for the test
execute_process(COMMAND tar -xzvf ${PROJECT_SOURCE_DIR}/neuromapp/coreneuron_1.0/common/data/rhs_d_ref.tar.gz
                            -C    ${PROJECT_BINARY_DIR}/test/)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMake/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
                           ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           COMMENT "Generating API documentation with Doxygen" VERBATIM
                     )
endif()
