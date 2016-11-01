# - Try to find the SKV installation.
#
# The following are set after configuration is done: 
#  SKV_FOUND - True if SKV installation is found and SKV not disabled by the user
#  SKV_INCLUDE_DIRS - SKV libraries path
#  SKV_LIBRARY_DIRS - SKV headers path
#  SKV_LIBRARIES - List of libraries when using SKV

# Use 'cmake -DNEUROMAPP_DISABLE_SKV=TRUE' to disable SKV
# or add 'PATH_TO_SKV_ROOT_DIR' to CMAKE_PREFIX_PATH'
# or set 'SKV_PATH=PATH_TO_SKV_ROOT_DIR to help cmake find the SKV installation

if (NOT NEUROMAPP_DISABLE_SKV)
    find_path(SKV_INCLUDE_DIR NAMES skv/client/skv_client.hpp
              HINTS ${CMAKE_PREFIX_PATH}/include $ENV{SKV_PATH}/include)
    find_library(SKV_LIBRARY_CLIENT NAMES skv_client_mpi
              HINTS ${CMAKE_PREFIX_PATH}/lib $ENV{SKV_PATH}/lib)
    find_library(SKV_LIBRARY_COMMON NAMES skv_common
              HINTS ${CMAKE_PREFIX_PATH}/lib $ENV{SKV_PATH}/lib)
    find_library(SKV_LIBRARY_API NAMES it_api
              HINTS ${CMAKE_PREFIX_PATH}/lib $ENV{SKV_PATH}/lib)
    find_library(SKV_LIBRARY_LOGGER NAMES fxlogger
              HINTS ${CMAKE_PREFIX_PATH}/lib $ENV{SKV_PATH}/lib)

    set(SKV_LIBRARIES ${SKV_LIBRARY_CLIENT} ${SKV_LIBRARY_COMMON} ${SKV_LIBRARY_API} ${SKV_LIBRARY_LOGGER})
    set(SKV_INCLUDE_DIRS ${SKV_INCLUDE_DIR})
    get_filename_component(SKV_LIBRARY_DIRS "${SKV_LIBRARY_CLIENT}" PATH)

    include(FindPackageHandleStandardArgs)
    # Handle the QUIETLY and REQUIRED arguments and set the SKV_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(SKV DEFAULT_MSG SKV_LIBRARY_CLIENT SKV_LIBRARY_COMMON SKV_LIBRARY_API SKV_LIBRARY_LOGGER SKV_INCLUDE_DIR)

    mark_as_advanced(SKV_INCLUDE_DIR SKV_LIBRARY_CLIENT SKV_LIBRARY_COMMON SKV_LIBRARY_API SKV_LIBRARY_LOGGER)
else()
    set(SKV_FOUND "FALSE")
    set(SKV_INCLUDE_DIRS "")
    set(SKV_LIBRARY_DIRS "")
    set(SKV_LIBRARIES "")
endif()


# Print summary
if(SKV_FOUND)
    message("SKV include dir = ${SKV_INCLUDE_DIR}")
    message("SKV lib = ${SKV_LIBRARY_CLIENT}")
    
    message("SKV_FOUND: ${SKV_FOUND}")
    message("SKV_INCLUDE_DIRS: ${SKV_INCLUDE_DIRS}")
    message("SKV_LIBRARY_DIRS: ${SKV_LIBRARY_DIRS}")
    message("SKV_LIBRARIES: ${SKV_LIBRARIES}")
else()
    if(NEUROMAPP_DISABLE_SKV)
        message(STATUS "SKV disabled by user.")
    else(NEUROMAPP_DISABLE_SKV)
        message(STATUS "Couldn't find SKV library, ignoring it.")
    endif(NEUROMAPP_DISABLE_SKV)
endif()
