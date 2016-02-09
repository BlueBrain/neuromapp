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

namespace spike {

/*
void spike_exchange(mpi_spike_graph &g){
    g.load_send_buf();
    //gather how many events each neighbor is sending
    g.allgather();
    //get the displacements
    g.set_displ();
    //next distribute items to every other process using allgatherv
    g.allgatherv();
}
*/
void allgather_int(mpi_spike_graph &g, int size, int_vec nin){
    g.allgather(size, nin);
}

void allgatherv_spike(mpi_spike_graph &g,
spike_vec spikeout, int_vec nin, int_vec displ, spike_vec spikein){
    g.allgatherv(spikeout, nin, displ, spikein);
}

void set_displ(int num_procs, const int_vec& nin, int_vec& displ){
    displ[0] = 0;
    int total = nin[0];
    for(int i=1; i < num_procs; ++i){
        displ[i] = total;
        total += nin[i];
    }
}

#endif
