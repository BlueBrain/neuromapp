add_definitions("-std=c++11")

list(APPEND readi_tests tets model reac comprej)

add_definitions("-DFILEPATH_TO_DATASET=\"${CMAKE_CURRENT_SOURCE_DIR}/example_dataset\"")

foreach(test_name IN LISTS readi_tests) 
    add_executable("readi_${test_name}" "${test_name}.cpp")
    target_link_libraries("readi_${test_name}" readi ${Boost_LIBRARIES})
    add_serial_test("readi_${test_name}")
endforeach()

