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
 * contains algorithm definitions for SpikeExchangeGraph
 */

#ifndef algos_h
#define algos_h

#include <assert.h>
#include <cstddef>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <mpi.h>

//#include "spike/spike_item.h"
//SPIKE TYPE
struct spike_item{
	int dst_;
	double t_;
};

inline MPI_Datatype create_spike_type(){
    MPI_Datatype spike;
    const int nblocks = 2;
    int blocklengths[2] = {1,1};
    MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};
    MPI_Aint offsets[2];

    offsets[0] = offsetof(spike_item, dst_);
    offsets[1] = offsetof(spike_item, t_);

    MPI_Type_create_struct(nblocks, blocklengths, offsets, types, &spike);
    MPI_Type_commit(&spike);
    return spike;
}

//BLOCKING
template<typename data>
void allgather(data& d){
    int size = d.spikeout_.size();
    MPI_Allgather(&size, 1, MPI_INT, &(d.nin_[0]), 1, MPI_INT, MPI_COMM_WORLD);
}

template<typename data>
void allgatherv(data& d, MPI_Datatype spike){
    MPI_Allgatherv(&(d.spikeout_[0]), d.spikeout_.size(), spike,
        &(d.spikein_[0]), &(d.nin_[0]), &(d.displ_[0]), spike, MPI_COMM_WORLD);
}


//NON-BLOCKING
template<typename data>
MPI_Request Iallgather(data& d){
    MPI_Request request;
    int size = d.spikeout_.size();
    MPI_Iallgather(&size, 1, MPI_INT, &(d.nin_[0]), 1, MPI_INT,
        MPI_COMM_WORLD, &request);
    return request;
}

template<typename data>
MPI_Request Iallgatherv(data& d, MPI_Datatype spike){
    MPI_Request request;
    MPI_Iallgatherv(&(d.spikeout_[0]), d.spikeout_.size(), spike,
        &(d.spikein_[0]), &(d.nin_[0]), &(d.displ_[0]), spike,
        MPI_COMM_WORLD, &request);
    return request;
}

inline int get_status(MPI_Request request){
    int flag;
    MPI_Request_get_status(request, &flag, MPI_STATUS_IGNORE);
    return flag;
}

inline void wait(MPI_Request request){
    MPI_Wait(&request, MPI_STATUS_IGNORE);
}


//DISTRIBUTED
/*
template<typename data>
MPI_COMM create_dist_graph(data& d){
    MPI_COMM neighborhood;
    MPI_Dist_graph_create_adjacent(MPI_COMM_WORLD, d.in_neighbors_.size(),
        &(d.in_neighbors_[0]), MPI_UNWEIGHTED, d.out_neighbors_.size(),
        &(d.out_neighbors_[0]), MPI_UNWEIGHTED, MPI_INFO_NULL,
        false, &neighborhood);
    return neighborhood;
}

template<typename data>
void neighbor_allgather(data& d, MPI_Comm neighborhood){
    MPI_Neighbor_allgather(&(d.spikeout_.size()), 1, MPI_INT,
        &(d.nin_[0]), 1, MPI_INT, neighborhood);
}

template<typename data>
void neighbor_allgatherv(data& d, MPI_Datatype spike, MPI_Comm neighborhood){
    MPI_Neighbor_allgatherv(&(d.spikeout_[0]), d.spikeout_.size(), spike,
        &(d.spikein_[0]), &(d.nin_[0]), &(d.displ_[0]), spike, neighborhood);
}
*/

//SIMULATIONS
template<typename data>
void blocking_spike(data& d, MPI_Datatype spike){
    //load spikeout with the spikes to be sent
    d.load_spikeout();
    u//gather how many spikes each process is sending
    allgather(d);
    //set the displacements
    d.set_displ();
    //next distribute items to every other process using allgatherv
    allgatherv(d, spike);
}

template<typename data>
void non_blocking_spike(data& d, MPI_Datatype spike){
    int num_tasks = 5;
    MPI_Request request;
    MPI_Request requestv;
    int flag = 0;
    d.load_spikeout();
    //check thresh
    request = Iallgather(d);
    for(int i = 0; i < num_tasks; ++i){
        //run task;
        usleep(10);
        //do iallgatherv only the first time flag is set
        if(!flag){
            flag = get_status(request);
            //if flag, prep then perform iallgatherv
            if(flag){
                d.set_displ();
                requestv = Iallgatherv(d, spike);
            }
        }
    }
    //if flag was not set during loop, wait for iallgather
    //request to finish, then perform iallgatherv
    if(!flag){
        wait(request);
        d.set_displ();
        requestv = Iallgatherv(d, spike);
    }
    wait(requestv);
}

template<typename data>
void run_sim(data& d, int simtime, bool non_blocking){
    MPI_Datatype spike = create_spike_type();
    for(int i = 0; i < simtime; ++i){
        if((i % 5) == 0){
            if(non_blocking)
                non_blocking_spike(d, spike);
            else
                blocking_spike(d, spike);

            d.all_matching();
        }
    }
    MPI_Type_free(&spike);
}

#endif
