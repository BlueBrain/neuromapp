/*
 * Neuromapp - mpispikegraph.h, Copyright (c), 2015,
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
 * @file neuromapp/spike/mpispikegraph.h
 * contains declaration for the mpi_spike_graph class
 */


#include <mpi.h>
#include <assert.h>
#include <vector>
#include "spike/utils/spike_item.h"

#ifndef mpispikegraph_h
#define mpispikegraph_h
namespace spike{

typedef std::vector<int> int_vec;
typedef std::vector<spike_item> spike_vec;

class mpi_spike_graph {
protected:
    virtual void allgather(int size, int_vec nin)=0;

    virtual void allgatherv(spike_vec& spikeout, spike_vec& spikein, int_vec& nin, int_vec& displ)=0;
};

class blocking_global_graph : mpi_spike_graph {
    /** \fn allgather()
        \brief performs an allgather operation for send_buf_ sizes
     */
    void allgather(int size, int_vec nin);

    /** \fn allgatherv()
        \brief performs and allgatherv operation of spike_items
     */
    void allgatherv(spike_vec& spikeout, int_vec& nin, int_vec& displ, spike_vec& spikein);
};

class distributed_graph : mpi_spike_graph {
private:
    MPI_Comm neighborhood_;
    int size_;
    int rank_;

public:
    std::vector<int> in_neighbors_;
    std::vector<int> out_neighbors_;

    distributed_graph(int size, int rank);

    void allgather(int size, int_vec& nin);

    void allgatherv(spike_vec& spikeout, spike_vec& spikein, int_vec& nin, int_vec& displ);

    void allgatherv(int_vec& intout, int_vec& intin, int_vec& nin, int_vec& displ);
};

#endif
