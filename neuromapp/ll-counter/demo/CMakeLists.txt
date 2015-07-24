include_directories(..)

add_custom_target(demos)

set(demos busywait_perf p8_llc)

foreach(demo_name IN LISTS demos)
    add_executable("${demo_name}" "${demo_name}.cc")
    target_link_libraries("${demo_name}" llc rt)
    add_dependencies(demos "${demo_name}")
endforeach()

