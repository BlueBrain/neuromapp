

macro(add_serial_test arg1)
    if(SLURM_FOUND)
        add_test(NAME "${arg1}_test" COMMAND ${SLURM_SRUN_COMMAND}
            --time=00:00:10 "${arg1}")
    else()
        add_test(${arg1}_test ${arg1})
    endif()
endmacro()


macro(add_mpi_test arg1)
    if(SLURM_FOUND)
        add_test(NAME "${arg1}_test" COMMAND ${SLURM_SRUN_COMMAND}
           "-n" ${MPIEXEC_MAX_NUMPROCS} --time=00:00:10 "./${arg1}")
    else()
        add_test(NAME "${arg1}_test" COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} "./${arg1}")
    endif()
endmacro()
