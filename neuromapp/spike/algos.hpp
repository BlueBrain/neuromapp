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

#include "coreneuron_1.0/queueing/queue.h"

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

#if MPI_VERSION < 3
/**
 * \fn Iallgather(data& d)
 * \brief performs the nonblocking collective, MPI_Iallgather
 * (not supported on current Blue Gene Q compiler settings)
 * \param d the data environment on which this algo is called
 * \return the MPI_Request for this non-blocking operation
 */
template<typename data>
MPI_Request Iallgather(data& d){
	std::cerr<<"Error: MPI_VERSION < 3. Non-blocking allgather not supported"<<std::endl;
	exit(EXIT_FAILURE);
	return MPI_REQUEST_NULL;
}

/**
 * \fn Iallgatherv(data& d, MPI_Datatype spike)
 * \brief performs the nonblocking collective, MPI_Iallgatherv
 * (not supported on current Blue Gene Q compiler settings)
 * \param d the data environment on which this algo is called
 * \param spike the MPI_Datatype being communicated
 * \return the MPI_Request for this non-blocking operation
 */
template<typename data>
MPI_Request Iallgatherv(data& d, MPI_Datatype spike){
	std::cerr<<"Error: MPI_VERSION < 3. Non-blocking allgatherv not supported"<<std::endl;
	exit(EXIT_FAILURE);
	return MPI_REQUEST_NULL;
}
#else
//NON-BLOCKING
/**
 * \fn Iallgather(data& d)
 * \brief performs the nonblocking collective, MPI_Iallgather
 * \param d the data environment on which this algo is called
 * \return the MPI_Request for this non-blocking operation
 */
template<typename data>
MPI_Request Iallgather(data& d){
    MPI_Request request;
    int size = d.spikeout_.size();
    MPI_Iallgather(&size, 1, MPI_INT, &(d.nin_[0]), 1, MPI_INT,
        MPI_COMM_WORLD, &request);
    return request;
}

/**
 * \fn Iallgatherv(data& d, MPI_Datatype spike)
 * \brief performs the nonblocking collective, MPI_Iallgatherv
 * \param d the data environment on which this algo is called
 * \param spike the MPI_Datatype being communicated
 * \return the MPI_Request for this non-blocking operation
 */
template<typename data>
MPI_Request Iallgatherv(data& d, MPI_Datatype spike){
    MPI_Request request;
    MPI_Iallgatherv(&(d.spikeout_[0]), d.spikeout_.size(), spike,
        &(d.spikein_[0]), &(d.nin_[0]), &(d.displ_[0]), spike,
        MPI_COMM_WORLD, &request);
    return request;
}
#endif

/**
 * \fn get_status(MPI_Request request)
 * \brief non-blocking check to see if the request has completed
 * \param request the request being checked
 * \return the flag variable: 1 if complete, else 0
 */
inline int get_status(MPI_Request request){
    int flag;
    MPI_Request_get_status(request, &flag, MPI_STATUS_IGNORE);
    return flag;
}

/**
 * \fn wait(MPI_Request request)
 * \brief blocking wait for the request to complete
 * \param request the request to wait for
 */
inline void wait(MPI_Request request){
    MPI_Wait(&request, MPI_STATUS_IGNORE);
}

//SIMULATIONS
/**
 * \fn blocking_spike(data& d, MPI_Datatype spike)
 * \brief performs a blocking spike exchange
 * \param d the data environment on which this algo is called
 */
template<typename data>
void blocking_spike(data& d, MPI_Datatype spike){
    d.time_step();
    //gather how many spikes each process is sending
    allgather(d);
    //set the displacements
    d.set_displ();
    //next distribute items to every other process using allgatherv
    allgatherv(d, spike);
}

/**
 * \fn non_blocking_spike(data& d, MPI_Datatype spike)
 * \brief performs a nonblocking spike exchange
 * \param d the data environment on which this algo is called
 */
template<typename data>
void non_blocking_spike(data& d, MPI_Datatype spike){
    //a vector of function pointers for the parallel tasks
    boost::array<boost::function<void(data*)>, 4> parallel_tasks;
    parallel_tasks[0] = &data::parallel_send;
    parallel_tasks[1] = &data::parallel_enqueue;
    parallel_tasks[2] = &data::parallel_algebra;
    parallel_tasks[3] = &data::parallel_deliver;
    MPI_Request request;
    MPI_Request requestv;
    int flag = 0;

    //check thresh
    typename boost::array<boost::function<void(data*)>, 4>
        ::iterator it = parallel_tasks.begin();

    //perform send first then allgather
    (*it)(&d);
    ++it;
    request = Iallgather(d);
    while(it != parallel_tasks.end()){
        //run the remaining parallel tasks and check
        //the status of iallgather after each
        (*it)(&d);
        if(!flag){
            flag = get_status(request);
            //if flag, prepare then perform iallgatherv
            if(flag){
                d.set_displ();
                requestv = Iallgatherv(d, spike);
            }
        }
        ++it;
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

/**
 * \fn run_sim(data& d, int simtime, bool non_blocking)
 * \brief runs the simulation with either a blocking or non-blocking
 *  spike exchange depending on the parameter "non_blocking"
 * \param d the data environment on which this algo is called
 * \param non_blocking specifies whether to use the blocking or non-blocking
 *  implementation of spike exchange.
 */
template<typename data>
void run_sim(data& d, int simtime, bool non_blocking){
    MPI_Datatype spike = create_spike_type();
    d.generate_all_events(simtime);
    for(int i = 1; i <= simtime; ++i){
        if((i % 5) == 0){
            //load spikeout with the spikes to be sent
            if(non_blocking)
                non_blocking_spike(d, spike);
            else
                blocking_spike(d, spike);

            d.filter();
            d.spikeout_.clear();
        }
        else{
            d.time_step();
        }
        d.increment_time();
    }
    MPI_Type_free(&spike);
}

#endif

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
