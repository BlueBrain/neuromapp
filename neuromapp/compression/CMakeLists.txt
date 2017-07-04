## using the hello tool as a template for the initial cmake instructions
# Use -DNEUROMAPP_DISABLE_comp_test=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_COMPRESSION_MAPP)
    add_library (compression main.cpp)
    if ( ZLIB_FOUND )
        ## this is according to cmake third parties SE post
        include_directories( ${ZLIB_INCLUDE_DIRS})
        target_link_libraries(compression ${ZLIB_LIBRARIES})
        set_target_properties(compression PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic-errors")
    endif (ZLIB_FOUND)

    ##currently hardcoded to directory with more recent version of boost than the system's
    ##currently makes the boost hana work this way, but attempting with CMAKE_PREFIX_PATH also

    #add_executable(prac_compression prac_compression.cpp)
    set_source_files_properties(prac_sort.cpp COMPILE_FLAGS "-Wall -Wextra -pedantic-errors")
    add_executable(prac_sort prac_sort.cpp)
    ##hana region
    set_source_files_properties(prac_hana.cpp COMPILE_FLAGS "-Wfatal-errors -I /home/lil/boost_1_64_0")
    ##hana needs c++14 so compile with this standard set
    add_executable(prac_hana prac_hana.cpp)
	set_property(TARGET prac_hana PROPERTY CXX_STANDARD 14)	
    ##shifting make region
    add_executable(prac_shift prac_shift.cpp)
    set_source_files_properties(prac_shift.cpp COMPILE_FLAGS "-Wall -Wextra -pedantic-errors -std=c++11")

    #target_link_libraries(prac_compression ${ZLIB_LIBRARIES})

    install (TARGETS compression DESTINATION lib)
    install (FILES block.h DESTINATION include)

    set(NEUROMAPP_COMPRESSION_MAPP ON CACHE BOOL "True if comp_test mini-app will be installed")
else()
    set(NEUROMAPP_COMPRESSION_MAPP OFF CACHE BOOL "True if comp_test mini-app will be installed")
endif()


