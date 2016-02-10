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
 * contains declaration for the spike_exchange class
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

class spike_exchange {
public:
    virtual void allgather(const int size, int_vec& nin)=0;

    virtual void allgatherv(const spike_vec& spikeout, spike_vec& spikein,
    const int_vec& nin, const int_vec& displ)=0;

    virtual MPI_Request Iallgather(const int size, int_vec& nin, MPI_Request request)=0;

    virtual MPI_Request Iallgatherv(const spike_vec& spikeout, spike_vec& spikein,
    const int_vec& nin, const int_vec& displ, MPI_Request request)=0;

    virtual int get_status(MPI_Request request, int flag)=0;

    virtual void wait(MPI_Request request)=0;
};

class global_collective : public spike_exchange {
private:

public:
    MPI_Datatype mpi_spike_item_;
    global_collective();

    /** \fn allgather()
        \brief performs an allgather operation for send_buf_ sizes
     */
    void allgather(const int size, int_vec& nin);

    /** \fn allgatherv()
        \brief performs and allgatherv operation of spike_items
     */
    void allgatherv(const spike_vec& spikeout, spike_vec& spikein,
    const int_vec& nin, const int_vec& displ);

    MPI_Request Iallgather(const int size, int_vec& nin, MPI_Request request);

    MPI_Request Iallgatherv(const spike_vec& spikeout, spike_vec& spikein,
    const int_vec& nin, const int_vec& displ, MPI_Request request);

    int get_status(MPI_Request request, int flag);

    void wait(MPI_Request request);
};

class distributed : public spike_exchange {
private:
    MPI_Comm neighborhood_;
    int size_;
    int rank_;

public:
    MPI_Datatype mpi_spike_item_;
    int_vec in_neighbors_;
    int_vec out_neighbors_;

    distributed(int size, int rank);

    void allgather(const int size, int_vec& nin);

    void allgatherv(const spike_vec& spikeout, spike_vec& spikein,
    const int_vec& nin, const int_vec& displ);

    void allgatherv(const int_vec& intout, int_vec& intin,
    const int_vec& nin, const int_vec& displ);

    MPI_Request Iallgather(const int size, int_vec& nin, MPI_Request request);

    MPI_Request Iallgatherv(const spike_vec& spikeout, spike_vec& spikein,
    const int_vec& nin, const int_vec& displ, MPI_Request request);

    int get_status(MPI_Request request, int flag);

    void wait(MPI_Request request);
};

}
#endif
