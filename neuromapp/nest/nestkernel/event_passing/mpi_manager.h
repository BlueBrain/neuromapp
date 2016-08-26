/*
 * mpi_manager.h
 *
 *  Created on: Jul 6, 2016
 *      Author: schumann
 */

#ifndef MPI_MANAGER_H_
#define MPI_MANAGER_H_


#include <limits.h>
#include <vector>

#include <mpi.h>
#ifdef _OPENMP
    #include <omp.h>
#endif

typedef unsigned int uint_t;

#define COMM_OVERFLOW_ERROR UINT_MAX

namespace nest
{
    namespace mpi_manager
    {
        void
        communicate_Allgather( std::vector< uint_t >& send_buffer,
          std::vector< uint_t >& recv_buffer,
          std::vector< int >& displacements,
          int& send_buffer_size,
          int& recv_buffer_size);

        void
        communicate( std::vector< uint_t >& send_buffer,
          std::vector< uint_t >& recv_buffer,
          std::vector< int >& displacements,
          int& send_buffer_size,
          int& recv_buffer_size);
    };
};


#endif /* MPI_MANAGER_H_ */
