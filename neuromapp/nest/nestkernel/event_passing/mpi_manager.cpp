/*
 * mpi_manager.h
 *
 *  Created on: Jul 6, 2016
 *      Author: schumann
 */
#include "nest/nestkernel/event_passing/mpi_manager.h"

void
nest::mpi_manager::communicate_Allgather( std::vector< uint_t >& send_buffer,
  std::vector< uint_t >& recv_buffer,
  std::vector< int >& displacements,
  int& send_buffer_size,
  int& recv_buffer_size)
{
    int num_processes;

    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &num_processes);
    int num_threads = omp_get_num_threads();

  std::vector< int > recv_counts( num_processes, send_buffer_size );

    // attempt Allgather
    if ( send_buffer.size() == static_cast< uint_t >( send_buffer_size ) )
    {
    MPI_Allgather( &send_buffer[ 0 ],
      send_buffer_size,
      MPI_UNSIGNED,
      &recv_buffer[ 0 ],
      send_buffer_size,
      MPI_UNSIGNED,
      comm );
    }
    else
    {
    // DEC cxx required 0U literal, HEP 2007-03-26
    std::vector< uint_t > overflow_buffer( send_buffer_size, 0U );
    overflow_buffer[ 0 ] = COMM_OVERFLOW_ERROR;
    overflow_buffer[ 1 ] = send_buffer.size();
    MPI_Allgather( &overflow_buffer[ 0 ],
      send_buffer_size,
      MPI_UNSIGNED,
      &recv_buffer[ 0 ],
      send_buffer_size,
      MPI_UNSIGNED,
      comm );
    }
    // check for overflow condition
    int disp = 0;
    uint_t max_recv_count = send_buffer_size;
    bool overflow = false;
    for ( int pid = 0; pid < num_processes; ++pid )
    {
    uint_t block_disp = pid * send_buffer_size;
    displacements[ pid ] = disp;
    if ( recv_buffer[ block_disp ] == COMM_OVERFLOW_ERROR )
    {
      overflow = true;
      recv_counts[ pid ] = recv_buffer[ block_disp + 1 ];
      if ( static_cast< uint_t >( recv_counts[ pid ] ) > max_recv_count )
      {
        max_recv_count = recv_counts[ pid ];
      }
    }
    disp += recv_counts[ pid ];
    }

    // do Allgatherv if necessary
    if ( overflow )
    {
    recv_buffer.resize( disp, 0 );
    MPI_Allgatherv( &send_buffer[ 0 ],
      send_buffer.size(),
      MPI_UNSIGNED,
      &recv_buffer[ 0 ],
      &recv_counts[ 0 ],
      &displacements[ 0 ],
      MPI_UNSIGNED,
      comm );
    send_buffer_size = max_recv_count;
    recv_buffer_size = send_buffer_size * num_processes;
    }
}

void
nest::mpi_manager::communicate( std::vector< uint_t >& send_buffer,
  std::vector< uint_t >& recv_buffer,
  std::vector< int >& displacements,
  int& send_buffer_size,
  int& recv_buffer_size)
{
    int num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    int num_threads = omp_get_num_threads();

    displacements.resize( num_processes, 0 );
    if ( num_processes == 1 ) {
        displacements[ 0 ] = 0;
        if ( static_cast< uint_t >( recv_buffer_size ) < send_buffer.size() )
        {
            recv_buffer_size = send_buffer_size = send_buffer.size();
            recv_buffer.resize( recv_buffer_size );
        }
        recv_buffer.swap( send_buffer );
    }
    else {
        communicate_Allgather( send_buffer, recv_buffer, displacements, send_buffer_size, recv_buffer_size );
    }
}
