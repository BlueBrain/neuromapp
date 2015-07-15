add_executable (app main.cpp)
target_link_libraries (app hello cnkernel ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG})
