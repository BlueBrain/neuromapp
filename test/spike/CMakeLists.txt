find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

add_executable(spiketest test.cpp)
target_link_libraries(spiketest ${MPI_LIBRARIES} distributedgraph ${Boost_LIBRARIES})

if (MPI_COMPILE_FLAGS)
	set_target_properties(spiketest PROPERTIES
		COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if (MPI_LINK_FLAGS)
	set_target_properties(spiketest PROPERTIES
		LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

if(SLURM_FOUND)
	add_test(NAME spiketest COMMAND ${SLURM_SRUN_COMMAND} --time=00:00:10 spiketest)
else()
	add_test(NAME spiketest COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS}
			spiketest ${MPIEXEC_MAX_NUMPROCS})
endif()

