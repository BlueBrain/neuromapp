/*
 * Neuromapp - distributed.hpp, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/event_passing/spike/distributed.hpp
 * contains algorithm definitions for distributed graph implementation
 */

#ifndef MAPP_DISTRIBUTED_H
#define MAPP_DISTRIBUTED_H

#include <assert.h>
#include <cstddef>
#include <vector>
#include <mpi.h>

#include "coreneuron_1.0/event_passing/queueing/queue.h"
#include "coreneuron_1.0/event_passing/spike/algos.hpp"

inline
MPI_Comm create_dist_graph(environment::presyn_maker& presyns, int nout){
    MPI_Comm neighborhood;
    int size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //passed in as argument but not used
    environment::input_presyn dummy_input;

    //create a temporary buffer for sending
    std::vector<int> sendbuf;
    std::vector<int> outNeighbors;
    std::vector<int> inNeighbors;

    //Every rank takes a turn to broadcast their output presyns
    //If receiver has a corresponding input presyn, add as inNeighor
    for(int i = 0; i < size; ++i){
        if(rank == i){
            for(int j = 0; j < presyns.get_nout(); ++j){
                sendbuf.push_back(presyns[j]);
            }
        }
        else{
            sendbuf.resize(nout);
        }
        MPI_Bcast(&sendbuf[0], nout, MPI_INT, i, MPI_COMM_WORLD);

        //add sender to inNeighbors if there is matching input presyn
        if(rank != i){
            for(int j = 0; j < nout; ++j){
                if(presyns.find_input(sendbuf[j], dummy_input)){
                    inNeighbors.push_back(i);
                    break;
                }
            }
        }
        sendbuf.clear();
    }

    int send_size;
    //Now every rank broadcasts their inNeighbors
    //If receiver is an inNeighbor, add sender as outNeighbor
    for(int i = 0; i < size; ++i){
        //Broadcast the send size
        if(rank == i){
            send_size = inNeighbors.size();
        }
        MPI_Bcast(&send_size, 1, MPI_INT, i, MPI_COMM_WORLD);

        //send outNeighbors
        if(rank == i){
            for(int i = 0; i < inNeighbors.size(); ++i){
                sendbuf.push_back(inNeighbors[i]);
            }
        }
        else{
            sendbuf.resize(send_size);
        }
        MPI_Bcast(&sendbuf[0], send_size, MPI_INT, i, MPI_COMM_WORLD);

        //add sender to outNeighbors if receiver is an inNeighbor
        if(rank != i){
            for(int j = 0; j < nout; ++j){
                if(sendbuf[j] == rank){
                    outNeighbors.push_back(i);
                    break;
                }
            }
        }
        sendbuf.clear();
    }

    MPI_Dist_graph_create_adjacent(MPI_COMM_WORLD, inNeighbors.size(),
        &inNeighbors[0], MPI_UNWEIGHTED, outNeighbors.size(),
        &outNeighbors[0], MPI_UNWEIGHTED, MPI_INFO_NULL,
        false, &neighborhood);
    return neighborhood;
}

template<typename data>
void neighbor_allgather(data& d, MPI_Comm neighborhood){
    int send_size = d.spikeout_.size();
    MPI_Neighbor_allgather(&send_size, 1, MPI_INT, &d.nin_[0], 1, MPI_INT, neighborhood);
}

template<typename data>
void neighbor_allgatherv(data& d, MPI_Datatype spike, MPI_Comm neighborhood){
    MPI_Neighbor_allgatherv(&d.spikeout_[0], d.spikeout_.size(), spike,
        &d.spikein_[0], &d.nin_[0], &d.displ_[0], spike, neighborhood);
}

//SIMULATIONS
/**
 * \fn distributed_spike(data& d, MPI_Datatype spike)
 * \brief performs a spike exchange for distributed graph
 * \param d the data environment on which this algo is called
 * \param spike, the MPI data type to be sent
 */
template<typename data>
void distributed_spike(data& d, MPI_Datatype spike, MPI_Comm neighborhood){
    //gather how many spikes each process is sending
    neighbor_allgather(d, neighborhood);
    //set the displacements
    set_displ(d);
    //next distribute items to every other process using allgatherv
    neighbor_allgatherv(d, spike, neighborhood);
}

#endif
