include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

add_executable (app driver.cpp main.cpp)
target_link_libraries (app
                       hello
		       queueing
                       coreneuron10_kernel
                       coreneuron10_solver
                       coreneuron10_cstep
                       storage
		       ${READLINE_LIBRARIES}
                       ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}
		       ${Boost_SYSTEM_LIBRARY_RELEASE}
		       ${Boost_ATOMIC_LIBRARY_RELEASE})
install (TARGETS app DESTINATION bin)
