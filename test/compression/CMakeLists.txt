#compression test
#add_executable(block_test compression.cpp) 
add_executable(block_read_test  block_reading_tests.cpp)
set_target_properties(block_read_test PROPERTIES COMPILE_FLAGS "-Wfatal-errors -g")
if ( ZLIB_FOUND )
    ## this is according to cmake third parties SE post
    include_directories( ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(block_read_test ${ZLIB_LIBRARIES})
endif()
#target_link_libraries(block_test ${Boost_LIBRARIES})
target_link_libraries(block_read_test ${Boost_LIBRARIES})
#add_serial_test(block_test)
add_serial_test(block_read_test)
