# Use -DNEUROMAPP_DISABLE_HDF5MAPP=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_HDF5MAPP)
    # Check HDF5 version
    set(HDF5_OK OFF)
    if (HDF5_FOUND)
        if (HDF5_VERSION)
            if(HDF5_VERSION VERSION_LESS "1.8.0" AND NOT NEUROMAPP_DISABLE_HDF5MAPP)
                message(STATUS "Some mini-apps require HDF5 version 1.8.0 or greater, but found version ${HDF5_VERSION}, ignoring them (HDF5 mini-app)")
            else()
                set(HDF5_OK ON)
            endif()
        else()
            set(HDF5_OK ON)
            message(STATUS "HDF5 version could not be determined, HDF5-dependent mini-apps may fail (HDF5 mini-app)")
        endif()
    else()
        message(STATUS "HDF5 needed by some mini-apps, but HDF5 not found, ignoring them (HDF5 mini-app)")
    endif()

    if (HDF5_OK)
        include_directories(${PROJECT_BINARY_DIR})
        include_directories(${PROJECT_SOURCE_DIR})

        add_subdirectory(drivers)
        add_subdirectory(data)

        add_library(h5read h5reader.cpp )

        target_link_libraries (h5read
                               ${MPI_CXX_LIBRARIES}
                               ${MPI_C_LIBRARIES}
                               ${Boost_LIBRARIES}
                               ${HDF5_LIBRARIES})

        target_include_directories(h5read PRIVATE ${HDF5_INCLUDE_DIRS})

        install (TARGETS h5read DESTINATION lib)

        set(NEUROMAPP_HDF5_MAPP ON CACHE BOOL "True if HDF5 mini-app will be installed")
    else()
        set(NEUROMAPP_HDF5_MAPP OFF CACHE BOOL "True if HDF5 mini-app will be installed")
    endif()
else()
    # HDF5 data needed by Nest mini-apps, check it must be installed
    if(NOT NEUROMAPP_DISABLE_NEST)
        add_subdirectory(data)
    endif()

    set(NEUROMAPP_HDF5_MAPP OFF CACHE BOOL "True if HDF5 mini-app will be installed")
endif()

