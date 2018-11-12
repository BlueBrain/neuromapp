include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

add_executable (app driver.cpp main.cpp)

# Hello mini-app
if(NEUROMAPP_HELLO_MAPP)
    target_link_libraries (app
                           hello)
    add_definitions(-DNEUROMAPP_HELLO_MAPP=1)
    message(STATUS "Adding hello mini-app")
else()
    add_definitions(-DNEUROMAPP_HELLO_MAPP=0)
    message(STATUS "Ignoring hello mini-app")
endif()

# Nest mini-apps
if(NEUROMAPP_NEST_MAPP)
    target_link_libraries (app
                           nest_synapse
                           nest_h5import_driver)
    add_definitions(-DNEUROMAPP_NEST_MAPP=1)
    message(STATUS "Adding Nest mini-apps")
else()
    add_definitions(-DNEUROMAPP_NEST_MAPP=0)
    message(STATUS "Ignoring Nest mini-apps")
endif()

# HDF5 mini-app
if(NEUROMAPP_HDF5_MAPP)
    target_link_libraries (app
                           h5read_driver)
    add_definitions(-DNEUROMAPP_HDF5_MAPP=1)
    message(STATUS "Adding HDF5 mini-app")
else()
    add_definitions(-DNEUROMAPP_HDF5_MAPP=0)
    message(STATUS "Ignoring HDF5 mini-app")
endif()

# CoreNeuron mini-apps
if(NEUROMAPP_CORENEURON_MAPP)
    target_link_libraries (app
                           coreneuron10_kernel
                           coreneuron10_solver
                           coreneuron10_cstep
                           coreneuron10_event
                           coreneuron10_queue
                           storage)
    add_definitions(-DNEUROMAPP_CORENEURON_MAPP=1)
   message(STATUS "Adding CoreNeuron mini-apps")
else()
    add_definitions(-DNEUROMAPP_CORENEURON_MAPP=0)
    message(STATUS "Ignoring CoreNeuron mini-apps")
endif()

# Keyvalue mini-app
if(NEUROMAPP_KEYVALUE_MAPP)
    target_link_libraries (app
                           keyvalue)
    add_definitions(-DNEUROMAPP_KEYVALUE_MAPP=1)
    message(STATUS "Adding keyvalue mini-app")
else()
    add_definitions(-DNEUROMAPP_KEYVALUE_MAPP=0)
    message(STATUS "Ignoring keyvalue mini-app")
endif()

# Replib mini-app
if(NEUROMAPP_REPLIB_MAPP)
    target_link_libraries (app
                           replib)
    add_definitions(-DNEUROMAPP_REPLIB_MAPP=1)
    message(STATUS "Adding replib mini-app")
else()
    add_definitions(-DNEUROMAPP_REPLIB_MAPP=0)
    message(STATUS "Ignoring replib mini-app")
endif()

# Iobench mini-app
if(NEUROMAPP_IOBENCH_MAPP)
    target_link_libraries (app
                           iobench)
    add_definitions(-DNEUROMAPP_IOBENCH_MAPP=1)
    message(STATUS "Adding iobench mini-app")
else()
    add_definitions(-DNEUROMAPP_IOBENCH_MAPP=0)
    message(STATUS "Ignoring iobench mini-app")
endif()

# Readi mini-app
if(NEUROMAPP_READI_MAPP)
    target_link_libraries (app
                           readi)
    add_definitions(-DNEUROMAPP_READI_MAPP=1)
    message(STATUS "Adding readi mini-app")
else()
    add_definitions(-DNEUROMAPP_READI_MAPP=0)
    message(STATUS "Ignoring readi mini-app")
endif()

# compression mini-app
if(NEUROMAPP_COMPRESSION_MAPP)
    target_link_libraries (app
                           compression)
    add_definitions(-DNEUROMAPP_COMPRESSION_MAPP=1)
    message(STATUS "Adding compression mini-app")
else()
    add_definitions(-DNEUROMAPP_COMPRESSION_MAPP=0)
    message(STATUS "Ignoring compression mini-app")
endif()

# Other needed libraries
target_link_libraries (app
                       ${READLINE_LIBRARY}
                       ${CURSES_CURSES_LIBRARY}
                       ${Boost_PROGRAM_OPTIONS_LIBRARIES}
                       ${Boost_CHRONO_LIBRARIES}
                       ${Boost_SYSTEM_LIBRARIES}
                       ${Boost_ATOMIC_LIBRARIES})

install (TARGETS app DESTINATION bin)
