/*
 * Neuromapp - algos.hpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/spike/algos.hpp
 * contains algorithm definitions for spike exchange
 */

#ifndef algos_h
#define algos_h

#include <assert.h>
#include <cstddef>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <mpi.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

#include "coreneuron_1.0/event_passing/queueing/queue.h"

//define events as spike_item
typedef queueing::event spike_item;

/**
 * \fn create_spike_type()
 * \brief creates an MPI_Datatype required for MPI
 *  communication of the spike_item struct
 * \return the new MPI_Datatype, spike
 */
inline MPI_Datatype create_spike_type(){
    MPI_Datatype spike;
    const int nblocks = 2;
    int blocklengths[2] = {1,1};
    MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};
    MPI_Aint offsets[2];

    offsets[0] = offsetof(spike_item, data_);
    offsets[1] = offsetof(spike_item, t_);

    MPI_Type_create_struct(nblocks, blocklengths, offsets, types, &spike);
    MPI_Type_commit(&spike);
    return spike;
}

/**
 * \fn barrier()
 * \brief performs an MPI_Barrier
 */
inline void barrier(){
    MPI_Barrier(MPI_COMM_WORLD);
}

//BLOCKING
/**
 * \fn allgather(data& d)
 * \brief performs the blocking collective, MPI_Allgather
 * \param d the data environment on which this algo is called
 */
template<typename data>
void allgather(data& d){
    int size = d.spikeout_.size();
    MPI_Allgather(&size, 1, MPI_INT, &(d.nin_[0]), 1, MPI_INT, MPI_COMM_WORLD);
}

/**
 * \fn allgatherv(data& d, MPI_Datatype spike)
 * \brief performs the blocking collective, MPI_Allgatherv
 * \param d the data environment on which this algo is called
 * \param spike the MPI_Datatype being communicated
 */
template<typename data>
void allgatherv(data& d, MPI_Datatype spike){
    MPI_Allgatherv(&(d.spikeout_[0]), d.spikeout_.size(), spike,
        &(d.spikein_[0]), &(d.nin_[0]), &(d.displ_[0]), spike, MPI_COMM_WORLD);
}

template<typename data>
void set_displ(data& d){
    d.displ_[0] = 0;
    int total = d.nin_[0];
    for(int i=1; i < d.nin_.size(); ++i){
        d.displ_[i] = total;
        total += d.nin_[i];
    }
    d.spikein_.resize(total);
    std::cout<<"TOTAL: "<<total<<std::endl;
}


//SIMULATIONS
/**
 * \fn blocking_spike(data& d, MPI_Datatype spike)
 * \brief performs a blocking spike exchange
 * \param d the data environment on which this algo is called
 */
template<typename data>
void blocking_spike(data& d, MPI_Datatype spike){
    //gather how many spikes each process is sending
    allgather(d);
    //set the displacements
    set_displ(d);
    //next distribute items to every other process using allgatherv
    allgatherv(d, spike);
}

/**
 * \fn run_sim(data& d)
 * \brief runs the simulation with either a blocking or non-blocking
 *  spike exchange depending on the parameter "non_blocking"
 * \param d the data environment on which this algo is called
 * \param non_blocking specifies whether to use the blocking or non-blocking
 *  implementation of spike exchange.
 *
template<typename data>
void run_sim(data& d){
    //create the spike type and free at end
    MPI_Datatype spike = create_spike_type();

    d.generate_all_events();
    for(int i = 0; i <= (d.simtime() / d.mindelay()); ++i){
        //load spikeout with the spikes to be sent
        d.fixed_step();
        blocking_spike(d, spike);
        d.filter();
        d.spikeout_.clear();
        d.spikein_.clear();
    }

    MPI_Type_free(&spike);
}
*/
#endif
