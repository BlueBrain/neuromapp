add_library (coreneuron10_kernel
            kernel/mechanism/NaTs2_t.c
            kernel/mechanism/ProbAMPANMDA_EMS.c
            kernel/mechanism/Ih.c
            common/memory/data_manager.c
            common/util/nrnthread_handler.c
            common/util/reader.c
            common/util/writer.c
            common/util/timer.c
            kernel/main.c)

add_library (coreneuron10_solver
             solver/hines.c
             common/util/reader.c
             common/util/nrnthread_handler.c
             common/util/timer.c
             solver/main.c)


add_subdirectory(test)

install (TARGETS coreneuron10_kernel coreneuron10_solver  DESTINATION lib)
install (FILES kernel/kernel.h solver/solver.h DESTINATION include)
