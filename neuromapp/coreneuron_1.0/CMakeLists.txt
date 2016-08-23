include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#generate the file for the in put, create in the build directory
configure_file("${PROJECT_SOURCE_DIR}/neuromapp/coreneuron_1.0/common/data/path.h.in"
               "${PROJECT_BINARY_DIR}/neuromapp/coreneuron_1.0/common/data/path.h")


add_library (coreneuron10_common STATIC
            common/memory/nrnthread.c
            common/memory/memory.c
            common/util/nrnthread_handler.c
            common/util/timer.c
            common/data/helper.cpp)


add_library (coreneuron10_kernel STATIC
            kernel/helper.c
            kernel/mechanism/NaTs2_t.c
            kernel/mechanism/ProbAMPANMDA_EMS.c
            kernel/mechanism/Ih.c
            kernel/main.c)


add_library (coreneuron10_solver STATIC
             solver/helper.c
             solver/hines.c
             solver/main.c)

add_library (coreneuron10_cstep STATIC
             cstep/helper.c
             cstep/main.c)

add_library (coreneuron10_queue STATIC
             queue/main.cpp)

target_link_libraries(coreneuron10_cstep coreneuron10_kernel coreneuron10_common coreneuron10_queue)

install (TARGETS coreneuron10_kernel coreneuron10_solver coreneuron10_cstep
                 coreneuron10_common coreneuron10_queue DESTINATION lib)

install (FILES  kernel/mechanism/mechanism.h
                kernel/kernel.h
                solver/solver.h
                cstep/cstep.h
                common/data/helper.h
                queue/tool/bin_queue.hpp
                queue/tool/bin_queue.ipp
                queue/tool/sptq_queue.hpp
                queue/tool/sptq_queue.ipp
                queue/tool/queue_helper.h.hpp
                DESTINATION include)

add_subdirectory (event_passing)
