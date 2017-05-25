## using the hello tool as a template for the initial cmake instructions
# Use -DNEUROMAPP_DISABLE_comp_test=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_COMPRESSION_MAPP)
    add_library (compression main.cpp)
    find_package(ZLIB REQUIRED)
    if ( ZLIB_FOUND )
        ## this is according to cmake third parties SE post
        include_directories( ${ZLIB_INCLUDE_DIRS})
        target_link_libraries(compression ${ZLIB_LIBRARIES})
    endif (ZLIB_FOUND)

    ##currently hardcoded to directory with more recent version of boost than the system's
    set_source_files_properties(prac_hana.cpp COMPILE_FLAGS "-Wfatal-errors -I /home/lil/boost_1_64_0")

    add_executable(prac_compression prac_compression.cpp)
    ##hana needs c++14 so compile with this standard set
    add_executable(prac_hana prac_hana.cpp)
	set_property(TARGET prac_hana PROPERTY CXX_STANDARD 14)	

    target_link_libraries(prac_compression ${ZLIB_LIBRARIES})

    install (TARGETS compression DESTINATION lib)
    install (FILES block.h DESTINATION include)

    set(NEUROMAPP_COMPRESSION_MAPP ON CACHE BOOL "True if comp_test mini-app will be installed")
else()
    set(NEUROMAPP_COMPRESSION_MAPP OFF CACHE BOOL "True if comp_test mini-app will be installed")
endif()


