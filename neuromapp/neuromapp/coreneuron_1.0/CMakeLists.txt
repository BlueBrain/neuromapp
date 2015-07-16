add_library (coreneuron10_kernel
             kernel/mechanism/NaTs2_t.c
             kernel/mechanism/ProbAMPANMDA_EMS.c
             kernel/mechanism/Ih.c
             common/memory/data_manager.c
             common/util/reader.c
             common/util/writer.c
             kernel/main.c)
