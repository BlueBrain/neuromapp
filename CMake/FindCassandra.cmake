# - Try to find the Cassandra installation.
#
# The following are set after configuration is done: 
#  CASSANDRA_FOUND - True if Cassandra installation is found and Cassandra not disabled by the user
#  CASSANDRA_INCLUDE_DIRS - Cassandra libraries path
#  CASSANDRA_LIBRARY_DIRS - Cassandra headers path
#  CASSANDRA_LIBRARIES - List of libraries when using Cassandra

# Use 'cmake -DNEUROMAPP_DISABLE_CASSANDRA=TRUE' to disable Cassandra
# or add 'PATH_TO_CASSANDRA_ROOT_DIR' to CMAKE_PREFIX_PATH'
# or set 'CASSANDRA_PATH=PATH_TO_CASSANDRA_ROOT_DIR to help cmake find the Cassandra installation

if (NOT NEUROMAPP_DISABLE_CASSANDRA)
    find_path(CASSANDRA_INCLUDE_DIR NAMES cassandra.h
               HINTS ${CMAKE_PREFIX_PATH}/include $ENV{CASSANDRA_PATH}/include)
    find_library(CASSANDRA_LIBRARY_CASS NAMES cassandra
              HINTS ${CMAKE_PREFIX_PATH}/lib $ENV{CASSANDRA_PATH}/lib)
              
    get_filename_component(CASSANDRA_LIBRARY_DIRS "${CASSANDRA_LIBRARY_CASS}" PATH)
              
    find_library(CASSANDRA_LIBRARY_UV NAMES uv
              HINTS ${CASSANDRA_LIBRARY_DIRS} ${CMAKE_PREFIX_PATH}/lib $ENV{CASSANDRA_PATH}/lib)

    set(CASSANDRA_LIBRARIES ${CASSANDRA_LIBRARY_CASS} ${CASSANDRA_LIBRARY_UV})
    set(CASSANDRA_INCLUDE_DIRS ${CASSANDRA_INCLUDE_DIR})
    get_filename_component(CASSANDRA_LIBRARY_DIRS "${CASSANDRA_LIBRARY_CASS}" PATH)

    include(FindPackageHandleStandardArgs)
    # Handle the QUIETLY and REQUIRED arguments and set the CASSANDRA_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(CASSANDRA DEFAULT_MSG CASSANDRA_LIBRARY_CASS CASSANDRA_LIBRARY_UV CASSANDRA_INCLUDE_DIR)

    mark_as_advanced(CASSANDRA_INCLUDE_DIR CASSANDRA_LIBRARY_CASS CASSANDRA_LIBRARY_UV)
else()
    set(CASSANDRA_FOUND "FALSE")
    set(CASSANDRA_INCLUDE_DIRS "")
    set(CASSANDRA_LIBRARY_DIRS "")
    set(CASSANDRA_LIBRARIES "")
endif()


# Print summary
if(CASSANDRA_FOUND)
    message("Cassandra include dir = ${CASSANDRA_INCLUDE_DIR}")
    message("Cassandra lib = ${CASSANDRA_LIBRARY_CASS}")
    
    message("CASSANDRA_FOUND: ${CASSANDRA_FOUND}")
    message("CASSANDRA_INCLUDE_DIRS: ${CASSANDRA_INCLUDE_DIRS}")
    message("CASSANDRA_LIBRARY_DIRS: ${CASSANDRA_LIBRARY_DIRS}")
    message("CASSANDRA_LIBRARIES: ${CASSANDRA_LIBRARIES}")
else()
    if(NEUROMAPP_DISABLE_CASSANDRA)
        message(STATUS "Cassandra disabled by user.")
    else(NEUROMAPP_DISABLE_CASSANDRA)
        message(STATUS "Couldn't find Cassandra library, ignoring it.")
    endif(NEUROMAPP_DISABLE_CASSANDRA)
endif()
