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
 * contains declaration for the MpiSpikeGraph class
 */


#include <mpi.h>
#include <assert.h>
#include <vector>
#include "spike/utils/spike_item.h"

#ifndef mpispikegraph_h
#define mpispikegraph_h
class MpiSpikeGraph {
protected:
    int num_procs_;
    int rank_;
    size_t events_per_;
    size_t num_out_;
    size_t num_in_;
    size_t sim_time_;
    static const size_t min_delay_ = 5;
    MPI_Datatype mpi_spike_item_;

public:
    //used in setup
    std::vector<int> input_presyns_;
    std::vector<int> output_presyns_;
    int total_received_;
    int total_relevent_;

    //used during collective functions
    std::vector<spike_item> generated_spikes_;
    std::vector<spike_item> send_buf_;
    std::vector<spike_item> recv_buf_;
    std::vector<int> size_buf_;
    std::vector<int> displ_;

    /** \fn MpiSpikeGraph(int n, int rank, size_t numOut, size_t numIn)
        \brief initializes an MpiSpikeGraph with input params
        \param n the number of MPI processes
        \param rank the rank of this processes
        \param numOut the number of output_presyns per process
        \param numIn the maximum number of Input_presyns per node
     */
MpiSpikeGraph(int n, int rank, size_t numOut, size_t numIn,
        size_t eventsPer, size_t simTime) : num_procs_(n),
        rank_(rank), total_received_(0), total_relevent_(0), num_out_(numOut),
	num_in_(numIn), events_per_(eventsPer), sim_time_(simTime)
        {
            mpi_spike_item_ = create_mpi_spike_type(mpi_spike_item_);
            srand(time(NULL)+rank_);
        }


    ~MpiSpikeGraph(){MPI_Type_free(&mpi_spike_item_);}

    /** \fn setup()
        \brief sets up the environment to prepare for spike exchange.
        \postcond each process fills in their input_presyns_ and output_presyns_ vectors
     */
    void setup();

    /** \fn generate_spikes()
     *  \brief generates all the spike_items to be used in the simulation
     */
    void generate_spikes();

    /** \fn allgather()
        \brief performs an allgather operation for send_buf_ sizes
     */
    void allgather();

    void set_displ();

    void load_send_buf();

    /** \fn allgatherv()
        \brief performs an allgatherv operation of spike_items
     */
    void allgatherv();

    /** \fn matches(sitem)
        \brief checks to see if an input spike_item was sent to one of my input presyns
         and if so, delivers the spike to that input presyn
     */
    bool matches(const spike_item &sitem);

    void reduce_stats();
};

class DistributedSpikeGraph : public MpiSpikeGraph {
private:
    MPI_Comm neighborhood_;

public:
    std::vector<int> in_neighbors_;
    std::vector<int> out_neighbors_;

    /** \fn DistributedSpikeGraph(int n, int rank, size_t numOut, size_t numIn)
        \brief type of MpiSpikeGraph where each node is connected to a subset of the
        entire graph (a neighborhood)
     */
    DistributedSpikeGraph(int n, int rank, size_t numOut,
    size_t numIn, size_t eventsPer, size_t simTime):
            MpiSpikeGraph(n, rank, numOut, numIn, eventsPer, simTime) {}

    /** \fn setup()
        \brief creates the local neighborhood for this process, to set up a distributed
        graph topology.
     */
    void setup();

    /** \fn allgather()
        \brief performs an allgather operation for send_buf_ sizes
     */
    void allgather();

    /** \fn allgatherv()
        \brief performs and allgatherv operation of spike_items
     */
    void allgatherv();
};

#endif
