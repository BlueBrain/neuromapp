# Use -DNEUROMAPP_DISABLE_NEST=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_NEST)
    # Check HDF5 version
    set(HDF5_OK OFF)
    if (HDF5_FOUND)
        if (HDF5_VERSION)
            if(HDF5_VERSION VERSION_LESS "1.8.0" AND NOT NEUROMAPP_DISABLE_HDF5MAPP)
                message(STATUS "Some mini-apps require HDF5 version 1.8.0 or greater, but found version ${HDF5_VERSION}, ignoring them (Nest mini-app)")
            else()
                set(HDF5_OK ON)
            endif()
        else()
            set(HDF5_OK ON)
            message(STATUS "HDF5 version could not be determined, HDF5-dependent mini-apps may fail (Nest mini-app)")
        endif()
    else()
        message(STATUS "HDF5 needed by some mini-apps, but HDF5 not found, ignoring them (Nest mini-app)")
    endif()

    if (HDF5_OK)
        include_directories(${HDF5_INCLUDE_DIRS})

        add_subdirectory (drivers)
        add_subdirectory (h5import)
        add_subdirectory (nestkernel)

        set(NEUROMAPP_NEST_MAPP ON CACHE BOOL "True if Nest mini-apps will be installed")
    else()
        set(NEUROMAPP_NEST_MAPP OFF CACHE BOOL "True if Nest mini-apps will be installed")
    endif()
else()
    set(NEUROMAPP_NEST_MAPP OFF CACHE BOOL "True if Nest mini-apps will be installed")
endif()

