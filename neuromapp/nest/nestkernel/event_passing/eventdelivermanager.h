/*
 * EventDeliveryManager.h
 *
 *  Created on: Jul 6, 2016
 *      Author: schumann
 */

#ifndef EVENTDELIVERYMANAGER_H_
#define EVENTDELIVERYMANAGER_H_

#include "nest/nestkernel/event_passing/mpi_manager.h"

#include "nest/nestkernel/environment/connectionmanager.h"

namespace nest
{
    class eventdelivermanager {
    private:
        /**
         * Register for gids of neurons that spiked. This is a 3-dim
         * structure.
         * - First dim: Each thread has its own vector to write to.
         * - Second dim: A vector for each slice of the min_delay interval
         * - Third dim: The gids.
         */
        std::vector< std::vector< std::vector< uint_t > > > spike_register_;

         /**
         * Buffer containing the gids of local neurons that spiked in the
         * last min_delay_ interval. The single slices are separated by a
         * marker value.
         */
        std::vector< uint_t > local_grid_spikes_;

         /**
         * Buffer containing the gids of all neurons that spiked in the
         * last min_delay_ interval. The single slices are separated by a
         * marker value
         */
        std::vector< uint_t > global_grid_spikes_;

        /**
           * Buffer containing the starting positions for the spikes from
           * each process within the global_(off)grid_spikes_ buffer.
           */
          std::vector< int > displacements_;


          int min_delay_;

          /**
           * Marker Value to be put between the data fields from different time
           * steps during communication.
           */
          const uint_t comm_marker_;

          int send_buffer_size_;
          int recv_buffer_size_;

          int num_threads_;
          int num_processes_;
          connectionmanager& cn_;

          void collocate_buffers_();
	  
	  void configure_spike_buffers();
    public:
          eventdelivermanager(connectionmanager& cn_, const unsigned int num_ranks, const unsigned int num_threads, const unsigned int min_delay);

        void gather_events();
        void deliver_events( thread thrd, long t );

        inline void
        send_remote( thread t, spikeevent& e, const long lag )
        {
            // Put the spike in a buffer for the remote machines
            spike_register_[ t ][ lag ].push_back( e.get_sender_gid() );
        }


    };
};

#endif /* EVENTDELIVERYMANAGER_H_ */
