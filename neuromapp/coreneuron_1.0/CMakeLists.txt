add_library (coreneuron10_common
            common/memory/data_manager.c
            common/util/nrnthread_handler.c
            common/util/reader.c
            common/util/writer.c
            common/util/timer.c)


add_library (coreneuron10_kernel
            kernel/helper.c
            kernel/mechanism/NaTs2_t.c
            kernel/mechanism/ProbAMPANMDA_EMS.c
            kernel/mechanism/Ih.c
            kernel/main.c)


add_library (coreneuron10_solver
             solver/helper.c
             solver/hines.c
             solver/main.c)

add_library (coreneuron10_cstep
             cstep/helper.c
             cstep/main.c)

target_link_libraries(coreneuron10_cstep coreneuron10_kernel coreneuron10_common)

install (TARGETS coreneuron10_kernel coreneuron10_solver coreneuron10_cstep coreneuron10_common DESTINATION lib)
install (FILES kernel/kernel.h solver/solver.h cstep/cstep.h DESTINATION include)
