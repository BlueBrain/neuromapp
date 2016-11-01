# - Try to find the CEPH installation.
#
# The following are set after configuration is done: 
#  CEPH_FOUND - True if CEPH installation is found and CEPH not disabled by the user
#  CEPH_INCLUDE_DIRS - CEPH libraries path
#  CEPH_LIBRARY_DIRS - CEPH headers path
#  CEPH_LIBRARIES - List of libraries when using CEPH

# Use 'cmake -DNEUROMAPP_DISABLE_CEPH=TRUE' to disable CEPH
# or add 'PATH_TO_CEPH_ROOT_DIR' to CMAKE_PREFIX_PATH'
# or set 'CEPH_PATH=PATH_TO_CEPH_ROOT_DIR to help cmake find the CEPH installation

if (NOT NEUROMAPP_DISABLE_CEPH)
    find_path(CEPH_INCLUDE_DIR NAMES rados/librados.hpp
               HINTS ${CMAKE_PREFIX_PATH}/include $ENV{CEPH_PATH}/include)
    find_library(CEPH_LIBRARY NAMES rados
              HINTS ${CMAKE_PREFIX_PATH}/lib64 $ENV{CEPH_PATH}/lib64)

    set(CEPH_LIBRARIES ${CEPH_LIBRARY})
    set(CEPH_INCLUDE_DIRS ${CEPH_INCLUDE_DIR})
    get_filename_component(CEPH_LIBRARY_DIRS "${CEPH_LIBRARY}" PATH)

    include(FindPackageHandleStandardArgs)
    # Handle the QUIETLY and REQUIRED arguments and set the CEPH_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(CEPH DEFAULT_MSG CEPH_LIBRARY CEPH_INCLUDE_DIR)

    mark_as_advanced(CEPH_INCLUDE_DIR CEPH_LIBRARY)
else()
    set(CEPH_FOUND "FALSE")
    set(CEPH_INCLUDE_DIRS "")
    set(CEPH_LIBRARY_DIRS "")
    set(CEPH_LIBRARIES "")
endif()


# Print summary
if(CEPH_FOUND)
    message("CEPH include dir = ${CEPH_INCLUDE_DIR}")
    message("CEPH lib = ${CEPH_LIBRARY}")
    
    message("CEPH_FOUND: ${CEPH_FOUND}")
    message("CEPH_INCLUDE_DIRS: ${CEPH_INCLUDE_DIRS}")
    message("CEPH_LIBRARY_DIRS: ${CEPH_LIBRARY_DIRS}")
    message("CEPH_LIBRARIES: ${CEPH_LIBRARIES}")
else()
    if(NEUROMAPP_DISABLE_CEPH)
        message(STATUS "CEPH disabled by user.")
    else(NEUROMAPP_DISABLE_CEPH)
        message(STATUS "Couldn't find CEPH library, ignoring it.")
    endif(NEUROMAPP_DISABLE_CEPH)
endif()
