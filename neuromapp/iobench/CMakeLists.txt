# Use -DNEUROMAPP_DISABLE_IOBENCH=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_IOBENCH)
    set(MPI_STATIC ON)
    find_package(MPI)

    if(MPI_FOUND)
        include_directories(${MPI_INCLUDE_PATH})
    endif()

    include_directories(${PROJECT_BINARY_DIR})
    include_directories(${PROJECT_SOURCE_DIR})

    #Add iobench to the mini-app library
    add_library (iobench main.cpp)

    install (TARGETS iobench DESTINATION lib)
    install (FILES iobench.h DESTINATION include)

    # LevelDB backend
    find_package(Leveldb)
    if (${LEVELDB_FOUND})
        include_directories(${LEVELDB_INCLUDE_DIRS})
        add_definitions(-DIO_LDB)

        install (FILES backends/leveldb.h DESTINATION include)
    else()
        set(LEVELDB_LIBRARIES "")
    endif()

    # Cassandra backend
    find_package(Cassandra)
    if (${CASSANDRA_FOUND})
        include_directories(${CASSANDRA_INCLUDE_DIRS})
        add_definitions(-DIO_CASS -DCASS_USE_OPENSSL -D_GNU_SOURCE)

        set(CASSANDRA_LIBRARIES ${CASSANDRA_LIBRARIES} "-lssl -lcrypto")

        install (FILES backends/cassandra.h DESTINATION include)
    else()
        set(CASSANDRA_LIBRARIES "")
    endif()
    # ADIOS backend
    find_package(ADIOS)
    if (${ADIOS_FOUND})
	    include_directories(${ADIOS_INCLUDE_DIRS})
    else()
        set(ADIOS_LIBRARIES "")
    endif()
    # OMP executable
    add_executable(iobench-omp iobench.cpp benchmark.cpp)
    target_link_libraries (iobench-omp ${ADIOS_LIBRARIES} ${LEVELDB_LIBRARIES} ${CASSANDRA_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARIES})

    if(MPI_FOUND)
        # MPI+OMP executable (add -DIO_MPI to compile flags)
        add_executable(MPI_Exec_io iobench.cpp benchmark.cpp)
        set_target_properties(MPI_Exec_io PROPERTIES
            COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -DIO_MPI")
        # Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
	target_link_libraries (MPI_Exec_io ${ADIOS_LIBRARIES} ${LEVELDB_LIBRARIES} ${CASSANDRA_LIBRARIES} ${MPI_CXX_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARIES})
    endif()

    install (TARGETS iobench-omp DESTINATION bin)
    if(MPI_FOUND)
        install (TARGETS MPI_Exec_io DESTINATION bin)
    endif()

    set(NEUROMAPP_IOBENCH_MAPP ON CACHE BOOL "True if iobench mini-app will be installed")
else()
    set(NEUROMAPP_IOBENCH_MAPP OFF CACHE BOOL "True if iobench mini-app will be installed")
endif()
