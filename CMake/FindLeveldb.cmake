# - Try to find the LevelDB installation.
#
# The following are set after configuration is done: 
#  LEVELDB_FOUND - True if LevelDB installation is found and LevelDB not disabled by the user
#  LEVELDB_INCLUDE_DIRS - LevelDB libraries path
#  LEVELDB_LIBRARY_DIRS - LevelDB headers path
#  LEVELDB_LIBRARIES - List of libraries when using LevelDB

# Use 'cmake -DNEUROMAPP_DISABLE_LEVELDB=TRUE' to disable LevelDB
# or add 'PATH_TO_LEVELDB_ROOT_DIR' to CMAKE_PREFIX_PATH'
# or set 'LEVELDB_PATH=PATH_TO_LEVELDB_ROOT_DIR to help cmake find the LevelDB installation

if (NOT NEUROMAPP_DISABLE_LEVELDB)
    find_path(LEVELDB_INCLUDE_DIR NAMES leveldb/db.h
               HINTS ${CMAKE_PREFIX_PATH}/include $ENV{LEVELDB_PATH}/include $ENV{LEVELDB_ROOT}/include)
    find_library(LEVELDB_LIBRARY NAMES leveldb
              HINTS ${CMAKE_PREFIX_PATH} $ENV{LEVELDB_PATH} $ENV{LEVELDB_ROOT}
              PATH_SUFFIXES lib out-shared out-static)

    set(LEVELDB_LIBRARIES ${LEVELDB_LIBRARY})
    set(LEVELDB_INCLUDE_DIRS ${LEVELDB_INCLUDE_DIR})
    get_filename_component(LEVELDB_LIBRARY_DIRS "${LEVELDB_LIBRARY}" PATH)

    include(FindPackageHandleStandardArgs)
    # Handle the QUIETLY and REQUIRED arguments and set the LEVELDB_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(LEVELDB DEFAULT_MSG LEVELDB_LIBRARY LEVELDB_INCLUDE_DIR)

    mark_as_advanced(LEVELDB_INCLUDE_DIR LEVELDB_LIBRARY)
else()
    set(LEVELDB_FOUND "FALSE")
    set(LEVELDB_INCLUDE_DIRS "")
    set(LEVELDB_LIBRARY_DIRS "")
    set(LEVELDB_LIBRARIES "")
endif()


# Print summary
if(LEVELDB_FOUND)
    message("LevelDB include dir = ${LEVELDB_INCLUDE_DIR}")
    message("LevelDB lib = ${LEVELDB_LIBRARY}")
    
    message("LEVELDB_FOUND: ${LEVELDB_FOUND}")
    message("LEVELDB_INCLUDE_DIRS: ${LEVELDB_INCLUDE_DIRS}")
    message("LEVELDB_LIBRARY_DIRS: ${LEVELDB_LIBRARY_DIRS}")
    message("LEVELDB_LIBRARIES: ${LEVELDB_LIBRARIES}")
else()
    if(NEUROMAPP_DISABLE_LEVELDB)
        message(STATUS "LevelDB disabled by user.")
    else(NEUROMAPP_DISABLE_LEVELDB)
        message(STATUS "Couldn't find LevelDB library, ignoring it.")
    endif(NEUROMAPP_DISABLE_LEVELDB)
endif()
