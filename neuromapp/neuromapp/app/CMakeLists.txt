add_executable (app main.cpp)
target_link_libraries (app
                       hello
                       coreneuron10_kernel
                       coreneuron10_solver
                       storage
                       ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG})
