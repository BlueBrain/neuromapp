# gtest-1.7.0: attempt to update/checkout gtest and then add to build

set(gtest_repo "http://googletest.googlecode.com/svn/tags/release-1.7.0")
set(gtest_src ${CMAKE_BINARY_DIR}/gtest-1.7.0)
set(gtest_bin ${CMAKE_CURRENT_BINARY_DIR}/gtest)

find_package(Subversion REQUIRED)
file(MAKE_DIRECTORY "${gtest_src}")
execute_process(COMMAND ${Subversion_SVN_EXECUTABLE} checkout "${gtest_repo}" "${gtest_src}")
add_subdirectory(${gtest_src} ${gtest_bin})

# tests are contained in sources test_XXX.cc

include_directories(".." "${gtest_src}/include")

add_custom_target(tests)

set(tests ll_timer tvalue_list unroll primitive_ops)

foreach(test_name IN LISTS tests)
    set(test_target "test_${test_name}")
    add_executable("${test_target}" "test_${test_name}.cc")
    target_link_libraries("${test_target}" gtest_main llc rt)
    add_test(NAME "${test_name}" COMMAND "${test_target}")
    add_dependencies(tests "${test_target}")
endforeach()

