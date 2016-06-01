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


#if MPI_VERSION >= 3
/**
 * \fn create_dist_graph(P& presyns, int ncells)
 * \brief Creates a distributed graph topology in order to perform nearest
 * neighbor communication.
 *
 *Summary:
 * - Uses MPI broadcast in order to exchange information about the cells
 *   on each ranks.
 *
 * - If you care about a cell on a different rank,
 *   add them as an inNeighbor.
 *
 * - Next exchange inNeighbor information.
 *
 * - If another rank has you as an inNeighbor, add them as an outNeighbor.
 *
 * - Use this information to construct topology.
 */
template <typename P>
MPI_Comm create_dist_graph(P& presyns, int ncells){
    MPI_Comm neighborhood;
    int size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //passed in as argument but not used
    environment::presyn dummy_input;

    //create a temporary buffer for sending
    std::vector<int> sendbuf;
    std::vector<int> outNeighbors;
    std::vector<int> inNeighbors;

    //Every rank takes a turn to broadcast their output presyns
    //If receiver has a corresponding input presyn, add as inNeighbor
    int start = 0;
    for(int i = 0; i < size; ++i){
        if(rank == i){
            start = rank * ncells;
            for(int j = 0; j < ncells; ++j){
                sendbuf.push_back(start + j);
            }
        }
        else{
            sendbuf.resize(ncells);
        }
        MPI_Bcast(&sendbuf[0], ncells, MPI_INT, i, MPI_COMM_WORLD);

        //add sender to inNeighbors if there is matching input presyn
        if(rank != i){
            for(int j = 0; j < ncells; ++j){
                if(presyns.find_input(sendbuf[j])){
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

        //send inNeighbors
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
            for(int j = 0; j < send_size; ++j){
                if(sendbuf[j] == rank){
                    outNeighbors.push_back(i);
                    break;
                }
            }
        }
        sendbuf.clear();
    }

    MPI_Dist_graph_create_adjacent(MPI_COMM_WORLD, inNeighbors.size(),
        &inNeighbors[0], (int*)MPI_UNWEIGHTED, outNeighbors.size(),
        &outNeighbors[0], (int*)MPI_UNWEIGHTED, MPI_INFO_NULL,
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
#else
/**
 * If MPI version is less than 3, there will be
 * compatibility issues, so use dummy functions.
 */
template <typename P>
MPI_Comm create_dist_graph(P& presyns, int ncells){
    std::cerr<<"MPI version is < 3. Cannot use distributed graph implementation"<<std::endl;
    exit(EXIT_FAILURE);
}

template<typename data>
void neighbor_allgather(data& d, MPI_Comm neighborhood){
    std::cerr<<"MPI version is < 3. Cannot use distributed graph implementation"<<std::endl;
    exit(EXIT_FAILURE);
}

template<typename data>
void neighbor_allgatherv(data& d, MPI_Datatype spike, MPI_Comm neighborhood){
    std::cerr<<"MPI version is < 3. Cannot use distributed graph implementation"<<std::endl;
    exit(EXIT_FAILURE);
}

template<typename data>
void distributed_spike(data& d, MPI_Datatype spike, MPI_Comm neighborhood){
    std::cerr<<"MPI version is < 3. Cannot use distributed graph implementation"<<std::endl;
    exit(EXIT_FAILURE);
}

#endif //MPI VERSION 3

#endif
