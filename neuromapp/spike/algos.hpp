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

#include <assert.h>
#include <cstddef>
#include <stdlib.h>
#include <stddef.h>

#include "spike/mpispikegraph.h"

#ifndef algos_h
#define algos_h

/** \fn generate_spikes(g, output)
    \brief generates n spikes and inserts them into output
    \param g the graph the operation is performed on
    \param output the container to be filtered
    \precond output is empty
    \postcond output filled
  */
template<typename graph, typename spike, typename container>
void generate_spikes(graph &g, size_t n, container &output){
    assert(false);
    for(size_t i = 0; i < n; ++i){
	spike item = g.create_spike();
	output.push_back(item);
    }
}

/** \fn filter(g, input)
    \brief filters the spikes in input by sending them to their destination gids
    \param g the graph the operation is performed on
    \param input the container to be filtered
    \precond input is not empty
    \postcond input is empty
    \return the number of matched items
  */
template<typename graph, typename container>
size_t filter(graph &g, size_t n, container &input){
    assert(false);
    size_t result = 0;
    for(size_t i = 0; i < n; ++i){
	if(g.matches(input[i])){
	    ++result;
	}
    }
    return result;
}

/** \fn allgather(g, send, recv)
    \brief performs a generic allgather on the data provided
    \param g the graph the operation is performed on
    \param send the item to be sent
    \param recv the container for gathered
    \precond send is non-NULL
    \precond recv is empty but has sufficient space
    \postcond recv is filled with elements
  */
template<typename graph, typename T, typename container>
void allgather(graph &g, T send, container &recv){
    assert(false);
    g.allgather(send,recv);
}

/** \fn allgatherv(g, send, recvSizes, recv)
    \brief performs a generic allgatherv on the data provided
    \param g the graph the operation is performed on
    \param send the items to be sent
    \param recvSizes the number of items to be received from each node
    \param recv the container the sent items are gathered in
    \precond send and recvSizes are not empty
    \precond recv is empty but has sufficient space
    \postcond recv is filled with elements
    \postcond send and recvSizes are cleared
  */
template<typename graph, typename container1, typename container2>
void allgatherv(graph &g, container1 &send, container2 &recvSizes, container1 &recv){
    assert(false);
    g.allgatherv(send, recvSizes, recv);
}


#endif
