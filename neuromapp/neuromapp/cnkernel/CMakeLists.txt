set(PATH_DATA_CNKERNEL ${CMAKE_SOURCE_DIR}/neuromapp/cnkernel/data)

add_library (cnkernel
             mechanism/NaTs2_t.c
             mechanism/ProbAMPANMDA_EMS.c
             memory/data_manager.c
             util/reader.c
             util/writer.c
             main.c)
