## using the hello tool as a template for the initial cmake instructions
# Use -DNEUROMAPP_DISABLE_compression=TRUE to disable compilation/installation of this mini-app
if(NEUROMAPP_ENABLE_COMPRESSION_MAPP)
    include_directories(${PROJECT_BINARY_DIR})
    include_directories(${PROJECT_SOURCE_DIR})
    configure_file("${PROJECT_SOURCE_DIR}/neuromapp/compression/common/data/path.h.in"
                   "${PROJECT_BINARY_DIR}/neuromapp/compression/common/data/path.h")
    add_library (compression main.cpp)
    if ( ZLIB_FOUND )
        ## this is according to cmake third parties SE post
        include_directories( ${ZLIB_INCLUDE_DIRS})
        target_link_libraries(compression ${ZLIB_LIBRARIES})
    endif (ZLIB_FOUND)

    set_target_properties(compression PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic-errors")

    install (TARGETS compression DESTINATION lib)
    install (FILES block.h DESTINATION include)

    set(NEUROMAPP_COMPRESSION_MAPP ON CACHE BOOL "True if compression mini-app will be installed")
else()
    set(NEUROMAPP_COMPRESSION_MAPP OFF CACHE BOOL "True if compression mini-app will be installed")
endif()


