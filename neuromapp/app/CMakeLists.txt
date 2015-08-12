add_executable (app driver.cpp main.cpp)
target_link_libraries (app
                       hello
                       coreneuron10_kernel
                       coreneuron10_solver
                       coreneuron10_cstep
                       storage
                       ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG})

install (TARGETS app DESTINATION bin)
