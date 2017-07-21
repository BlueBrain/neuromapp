## using the hello tool as a template for the initial cmake instructions
# Use -DNEUROMAPP_DISABLE_comp_test=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_COMPRESSION_MAPP)
    add_library (compression main.cpp)
    add_executable(kernel_run kernel_measurements.cpp)
    if ( ZLIB_FOUND )
        ## this is according to cmake third parties SE post
        include_directories( ${ZLIB_INCLUDE_DIRS})
        target_link_libraries(compression ${ZLIB_LIBRARIES})
        target_link_libraries(kernel_run ${ZLIB_LIBRARIES})
    endif (ZLIB_FOUND)

    set_target_properties(compression PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic-errors")
    set_target_properties(kernel_run PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic-errors")

    install (TARGETS compression DESTINATION lib)
    install (FILES block.h DESTINATION include)

    set(NEUROMAPP_COMPRESSION_MAPP ON CACHE BOOL "True if comp_test mini-app will be installed")
else()
    set(NEUROMAPP_COMPRESSION_MAPP OFF CACHE BOOL "True if comp_test mini-app will be installed")
endif()


