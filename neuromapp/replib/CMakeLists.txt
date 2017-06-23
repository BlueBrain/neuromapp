# Use -DNEUROMAPP_DISABLE_REPLIB=TRUE to disable compilation/installation of this mini-app
if(NOT NEUROMAPP_DISABLE_REPLIB)
    set(MPI_STATIC ON)
    find_package(MPI)
    if (${COMPILE_LIBRARY_TYPE} STREQUAL "STATIC")
      set(ADIOS_USE_STATIC_LIBS ON)
    endif()
    find_package (ADIOS)
    message ("       ADIOS LIBRARIES = ${ADIOS_LIBRARIES}")
    if (MPI_FOUND)
      set (ADIOS_FLAG "")

       include_directories(${MPI_INCLUDE_PATH})

        include_directories(${PROJECT_BINARY_DIR})
        include_directories(${PROJECT_SOURCE_DIR})

        #Add replib to the mini-app library
        add_library (replib main.cpp)
        install (TARGETS replib DESTINATION lib)
        install (FILES replib.h DESTINATION include)

        # Add statistics as a separate library
        add_library (replib-stats utils/statistics.cpp utils/tools.cpp)
        install (TARGETS replib-stats DESTINATION lib)

        add_executable(MPI_Exec_rl mpiexec.cpp )

        # Adding MPI_LIBRARIES adds also the -Bdynamic flag, which makes execution crash on BG/Q
        target_link_libraries (MPI_Exec_rl replib-stats ${MPI_CXX_LIBRARIES})
        
        # ADIOS option
        if (ADIOS_FOUND)
          include_directories(${ADIOS_INCLUDE_DIRS})
          target_link_libraries (MPI_Exec_rl ${ADIOS_LIBRARIES} replib-stats )
          set(ADIOS_FLAG -DRL_ADIOS=1)
        endif()

        set_target_properties(MPI_Exec_rl PROPERTIES
          COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS} ${ADIOS_FLAG}")

        install (TARGETS MPI_Exec_rl DESTINATION bin)

        # Install script to generate input distribution file
        install (PROGRAMS utils/replib_input_distr_generator.sh utils/uniform_generator.py utils/generate_set.py DESTINATION bin)
        install (PROGRAMS utils/uniform_distribution.py DESTINATION bin)
        install (PROGRAMS utils/uniform_distribution.py DESTINATION bin)

        set(NEUROMAPP_REPLIB_MAPP ON CACHE BOOL "True if replib mini-app will be installed")
    else()
        set(NEUROMAPP_REPLIB_MAPP OFF CACHE BOOL "True if replib mini-app will be installed")
        message(STATUS "MPI needed by some mini-apps, but MPI not found, ignoring them (replib mini-app)")
    endif()
else()
    set(NEUROMAPP_REPLIB_MAPP OFF CACHE BOOL "True if replib mini-app will be installed")
endif()
