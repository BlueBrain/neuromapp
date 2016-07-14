/*
 * EventDeliveryManager.cpp
 *
 *  Created on: Jul 6, 2016
 *      Author: schumann
 */

#include <vector>

#include "nest/eventdelivermanager.h"
#include "nest/mpi_manager.h"

using namespace nest;

eventdelivermanager::eventdelivermanager(connectionmanager& cn, const unsigned int num_ranks, const unsigned int num_threads, const unsigned int min_delay):
    min_delay_(min_delay),
    comm_marker_(-2), // in nest 0 is used as maker, in the miniapp neuron with gid 0 can exists, therefore the marker is changed
    send_buffer_size_( 1 ),
    recv_buffer_size_( 1 ),
    num_threads_(num_threads),
    num_processes_(num_ranks),
    displacements_(num_ranks),
    spike_register_(num_threads, std::vector< std::vector< uint_t > >(min_delay)),
    cn_(cn)
{
  configure_spike_buffers();
}

void
eventdelivermanager::collocate_buffers_()
{
    // count number of spikes in registers
    int num_spikes = 0;
    int num_grid_spikes = 0;

    std::vector< std::vector< std::vector< uint_t > > >::iterator i;
    std::vector< std::vector< uint_t > >::iterator j;
        for ( i = spike_register_.begin(); i != spike_register_.end(); ++i )
            for ( j = i->begin(); j != i->end(); ++j )
                num_grid_spikes += j->size();

    //skip num_offgrid_spikes
    //skip uintsize_secondary_events

    // +1 because we need one end marker invalid_synindex
    // +1 for bool-value done
    num_spikes = num_grid_spikes + 2; // + num_offgrid_spikes + uintsize_secondary_events


    if ( global_grid_spikes_.size() != recv_buffer_size_)
        global_grid_spikes_.resize(recv_buffer_size_, 0 );

    if ( num_spikes + ( num_threads_ * min_delay_ ) > static_cast< uint_t >( send_buffer_size_ ) )
          local_grid_spikes_.resize( num_spikes + ( min_delay_ * num_threads_ ) , 0 );
    else if ( local_grid_spikes_.size() < static_cast< uint_t >( send_buffer_size_ ) )
          local_grid_spikes_.resize(send_buffer_size_, 0 );

    // collocate the entries of spike_registers into local_grid_spikes__
    std::vector< uint_t >::iterator pos = local_grid_spikes_.begin();
    for ( i = spike_register_.begin(); i != spike_register_.end(); ++i )
        for ( j = i->begin(); j != i->end(); ++j )
        {
          pos = std::copy( j->begin(), j->end(), pos );
          *pos = comm_marker_;
          ++pos;
        }

    // remove old spikes from the spike_register_
    for ( i = spike_register_.begin(); i != spike_register_.end(); ++i )
      for ( j = i->begin(); j != i->end(); ++j )
        j->clear();

    //not sure if needed
    //// end marker after last secondary event
    //// made sure in resize that this position is still allocated
    //write_to_comm_buffer( invalid_synindex, pos );
    //// append the boolean value indicating whether we are done here
    //write_to_comm_buffer( done, pos );
}

void
eventdelivermanager::gather_events()
{
    collocate_buffers_();
    mpi_manager::communicate(local_grid_spikes_, global_grid_spikes_, displacements_, send_buffer_size_, recv_buffer_size_);
}

void
eventdelivermanager::deliver_events( thread thrd, long t )
{
    spikeevent se;

    std::vector< int > pos( displacements_ );

    // prepare Time objects for every possible time stamp within min_delay_
    std::vector< Time > prepared_timestamps( min_delay_ );
    for ( size_t lag = 0;
          lag < ( size_t ) min_delay_;
          lag++ )
    {
      assert(t>=lag);
      const long curTime = t - lag;
      prepared_timestamps[ lag ] = curTime;
    }

    for ( size_t vp = 0;
          vp < ( size_t ) (num_processes_ * num_threads_ );
          ++vp )
    {
      size_t pid = vp % num_processes_;
      int pos_pid = pos[ pid ];
      int lag = min_delay_ - 1;
      while ( lag >= 0 )
      {
        index nid = global_grid_spikes_[ pos_pid ];
        if ( nid != static_cast< index >( comm_marker_ ) )
        {
          // tell all local nodes about spikes on remote machines.
          se.set_stamp( prepared_timestamps[ lag ] );
          se.set_sender_gid( nid );
          std::cout <<"send " << thrd << " " << nid << " " << se.stamp_.get_ms() << std::endl;
          cn_.send( thrd, nid, se );
        }
        else
        {
          --lag;
        }
        ++pos_pid;
      }
      pos[ pid ] = pos_pid;
    }
    // skipped the secondary events
}


void
eventdelivermanager::configure_spike_buffers()
{
  assert( min_delay_ != 0 );

  spike_register_.clear();
  // the following line does not compile with gcc <= 3.3.5
  spike_register_.resize( num_threads_,
    std::vector< std::vector< uint_t > >( min_delay_ ) );
  for ( size_t j = 0; j < spike_register_.size(); ++j )
    for ( size_t k = 0; k < spike_register_[ j ].size(); ++k )
      spike_register_[ j ][ k ].clear();

  // send_buffer must be >= 2 as the 'overflow' signal takes up 2 spaces
  // plus the fiunal marker and the done flag for iterations
  // + 1 for the final markers of each thread (invalid_synindex) of secondary
  // events
  // + 1 for the done flag (true) of each process
  send_buffer_size_ = num_threads_ * min_delay_ + 2 > 4 ? num_threads_ * min_delay_ + 2 : 4;
  recv_buffer_size_ = send_buffer_size_ * num_processes_;

  // DEC cxx required 0U literal, HEP 2007-03-26
  local_grid_spikes_.clear();
  local_grid_spikes_.resize( send_buffer_size_, 0U );

  global_grid_spikes_.clear();
  global_grid_spikes_.resize( recv_buffer_size_, 0U );

  //// insert the end marker for payload event (==invalid_synindex)
  //// and insert the done flag (==true)
  //// after min_delay 0's (== comm_marker)
  //// use the template functions defined in event.h
  //// this only needs to be done for one process, because displacements is set to
  //// 0 so all processes initially read out the same positions in the global
  //// spike buffer
  //std::vector< uint_t >::iterator pos = global_grid_spikes_.begin()
  //  + kernel().vp_manager.get_num_threads()
  //    * kernel().connection_manager.get_min_delay();
  //write_to_comm_buffer( invalid_synindex, pos );
  //write_to_comm_buffer( true, pos );

  displacements_.clear();
  displacements_.resize( num_processes_, 0 );
}

