#include "nest/h5import/fakenestkernel/nest_kernel.h"
#include "nest/h5import/H5Synapses.h"

#include <iostream>
//#include "nmpi.h"
#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include <sstream>
#include <sys/types.h>
#include <ctime>
#include <sys/time.h>
#include <stdio.h>
#include <queue>

#ifdef IS_BLUEGENE_Q
#include <spi/include/kernel/memory.h>
#endif

#include <algorithm>

#ifdef SCOREP_USER_ENABLE
#ifndef SCOREP_COMPILE
#define SCOREP_COMPILE
#endif
#include <scorep/SCOREP_User.h>
#endif

using namespace h5import;

H5Synapses::H5Synapses()
  : transfersize_(524288), sizelimit_(-1)
{
	assert(kernel_available());
}

void H5Synapses::threadConnectNeurons( SynapseList& synapses )
{
#ifdef SCOREP_COMPILE
  SCOREP_USER_REGION( "connect", SCOREP_USER_REGION_TYPE_FUNCTION )
#endif

    #pragma omp parallel default( shared )
    {
        for ( int i = 0; i < synapses.size(); i++ )
        {
            const index target = synapses[ i ].target_neuron_;
            // synapse belongs to local thread, connect function is thread safe under this condition
            if ( kernel().node_manager.is_local_gid(target))
                singleConnect( synapses[ i ], target );
        }
    }
}

/**
 *  Communicate Synpases between the nodes
 *  Aftewards all synapses are on their target nodes
 */
CommunicateSynapses_Status
H5Synapses::CommunicateSynapses( SynapseList& synapses )
{
#ifdef SCOREP_COMPILE
  SCOREP_USER_REGION( "alltoall", SCOREP_USER_REGION_TYPE_FUNCTION )
#endif
  uint32_t num_processes = kernel().mpi_manager.get_num_processes();

  int sendcounts[ num_processes ], recvcounts[ num_processes ],
    rdispls[ num_processes + 1 ], sdispls[ num_processes + 1 ];
  for ( int32_t i = 0; i < num_processes; i++ )
  {
    sendcounts[ i ] = 0;
    sdispls[ i ] = 0;
    recvcounts[ i ] = -999;
    rdispls[ i ] = -999;
  }

  const int intsizeof_entry = synapses.sizeof_entry()/sizeof(int);
  mpi_buffer<int> send_buffer(synapses.size() * intsizeof_entry, true);

  // store number of int values per entry
  int entriesadded;

  #pragma omp parallel for
  for ( size_t i = 0; i < synapses.size(); i++ )
  {
    const size_t offset = i * intsizeof_entry;
    // serialize entry
    entriesadded = synapses[ i ].serialize( send_buffer, offset );

    // save number of values added
    sendcounts[ synapses[ i ].node_id_ ] += entriesadded;
  }

  MPI_Alltoall(
    sendcounts, 1, MPI_INT, recvcounts, 1, MPI_INT, MPI_COMM_WORLD );

  rdispls[ 0 ] = 0;
  sdispls[ 0 ] = 0;
  for ( uint32_t i = 1; i < num_processes + 1; i++ )
  {
    sdispls[ i ] = sdispls[ i - 1 ] + sendcounts[ i - 1 ];
    rdispls[ i ] = rdispls[ i - 1 ] + recvcounts[ i - 1 ];
  }

  // use number of values per entry to determine number of recieved synapses
  const int32_t recv_synpases_count = rdispls[ num_processes ] / intsizeof_entry;

  // allocate recv buffer
  mpi_buffer<int> recvbuf( rdispls[ num_processes ], true );

  MPI_Alltoallv( send_buffer.begin(),
    sendcounts,
    sdispls,
    MPI_INT,
    recvbuf.begin(),
    recvcounts,
    rdispls,
    MPI_INT,
    MPI_COMM_WORLD );

  // fill entries in synapse list
  synapses.resize( recv_synpases_count );

  // fill synapse list with values from buffer
  #pragma omp parallel for
  for ( size_t i = 0; i < synapses.size(); i++ ) {
      const size_t offset = i * intsizeof_entry;
      synapses[ i ].deserialize( recvbuf, offset );
  }
  // return status
  if ( sdispls[ num_processes ] > 0 && rdispls[ num_processes ] > 0 )
    return SENDRECV;
  else if ( sdispls[ num_processes ] > 0 )
    return SEND;
  else if ( rdispls[ num_processes - 1 ] > 0 )
    return RECV;
  else
    return NOCOM;
}

void H5Synapses::integrateMapping( SynapseList& synapses )
{
#ifdef SCOREP_COMPILE
  SCOREP_USER_REGION( "det", SCOREP_USER_REGION_TYPE_FUNCTION )
#endif

  for ( int i = 0; i < synapses.size(); i++ ) {
	  SynapseRef s = synapses[i];
      s.source_neuron_ = mapping_[ s.source_neuron_ ];
      s.target_neuron_ = mapping_[ s.target_neuron_ ];
      s.node_id_ = kernel().mpi_manager.suggest_rank( s.target_neuron_ );
  }
}

//helper for sort
typedef std::pair< int, int > intpair;
inline bool first_less( const intpair& l, const intpair& r ) { return l.first < r.first; };

void
H5Synapses::sort( SynapseList& synapses )
{
#ifdef SCOREP_COMPILE
    SCOREP_USER_REGION( "sort", SCOREP_USER_REGION_TYPE_FUNCTION )
#endif
    if ( synapses.size() > 1 )
    {
        // arg sort
        std::vector< intpair > v_idx( synapses.size() );
        for ( int i = 0; i < v_idx.size(); i++ )
        {
            v_idx[ i ].first = synapses[ i ].node_id_;
            v_idx[ i ].second = i;
        }
        std::sort( v_idx.begin(), v_idx.end(), first_less);

        // create buf object
        uint32_t source_neuron_tmp;
        uint32_t node_id_tmp;
        std::vector< char > pool_tmp( synapses.sizeof_entry() );
        SynapseRef buf( source_neuron_tmp,
                node_id_tmp,
                synapses.get_num_params(),
                &pool_tmp[0] );

        //apply reordering based on v_idx[:].second
        size_t i, j, k;
        for(i = 0; i < synapses.size(); i++){
            if(i != v_idx[ i ].second){
                buf = synapses[i];
                k = i;
                while(i != (j = v_idx[ k ].second)){
                    synapses[k] = synapses[j];
                    v_idx[ k ].second = k;
                    k = j;
                }
                synapses[k] = buf;
                v_idx[ k ].second = k;
            }
        }
    }
}

void H5Synapses::import()
{
  const int rank = kernel().mpi_manager.get_rank();
  const int size = kernel().mpi_manager.get_num_processes();

  CommunicateSynapses_Status com_status = UNSET;

  h5reader synloader( filename_,
                      model_params_,
                      transfersize_,
                      sizelimit_ );

  struct timeval start_mpicon, end_mpicon, start_load, end_load, start_push, end_push;
  uint64_t t_load=0;
  uint64_t t_mpicon=0;
  uint64_t t_push=0;
  std::queue< SynapseList* > synapse_queue;

  // add all synapses into queue
  gettimeofday(&start_push, NULL);
    #pragma omp parallel
    {
        #pragma omp single
        {
            while ( !synloader.eof() ) {
                #ifdef SCOREP_COMPILE
                SCOREP_USER_REGION( "enqueue", SCOREP_USER_REGION_TYPE_FUNCTION )
                #endif
                SynapseList* newone = new SynapseList( model_params_.size() );

                h5reader::h5view dataspace_view;
                {
                    #ifdef SCOREP_COMPILE
                    SCOREP_USER_REGION( "read", SCOREP_USER_REGION_TYPE_FUNCTION )
                    #endif

                    gettimeofday(&start_load, NULL);
                    synloader.readblock( *newone, dataspace_view );
                    gettimeofday(&end_load, NULL);
                }
                t_load += (1000 * (end_load.tv_sec - start_load.tv_sec))
                     + ((end_load.tv_usec - start_load.tv_usec) / 1000);

                #pragma omp task firstprivate(newone, dataspace_view)
                {
                    //synloader.integrateSourceNeurons( *newone, dataspace_view );
                    integrateMapping(*newone);
                    sort(*newone);
                }
                synapse_queue.push(newone);
            }
            #pragma omp taskwait
        }
    }
    gettimeofday(&end_push, NULL);

    //iterate over queue and connect connections in NEST data structure
    while (!synapse_queue.empty()) {
        #ifdef SCOREP_COMPILE
       SCOREP_USER_REGION( "dequeue", SCOREP_USER_REGION_TYPE_FUNCTION )
        #endif
        gettimeofday(&start_mpicon, NULL);
        SynapseList* synapses = synapse_queue.front();
        synapse_queue.pop();

        com_status = CommunicateSynapses( *synapses );
        threadConnectNeurons( *synapses );

        delete synapses;

        gettimeofday(&end_mpicon, NULL);
        t_mpicon += (1000 * (end_mpicon.tv_sec - start_mpicon.tv_sec))
            + ((end_mpicon.tv_usec - start_mpicon.tv_usec) / 1000);
    }
}
