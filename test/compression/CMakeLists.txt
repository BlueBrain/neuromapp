#compression test
set(tests compression_tests block_reading_tests sorting_tests spliting_tests)

foreach(i ${tests})
    add_executable(${i} ${i}.cpp)
    set_target_properties(${i} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic-errors")
    if ( ZLIB_FOUND )
        ## this is according to cmake third parties SE post
        include_directories( ${ZLIB_INCLUDE_DIRS})
        target_link_libraries(${i} ${ZLIB_LIBRARIES})
    endif()
    target_link_libraries(${i} ${Boost_LIBRARIES})
    add_serial_test(${i})
endforeach()
