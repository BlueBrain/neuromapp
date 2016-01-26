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

#ifndef mpispikegraph_h
#define mpispikegraph_h

struct SpikeItem{
	int dst_;
	double t_;
};

class MpiSpikeGraph {
protected:
	int num_procs_;
	int rank_;
	int total_received_;
	int total_relevent_;
	size_t events_per_;
	size_t num_out_;
	size_t num_in_;
	MPI_Datatype mpi_spikeItem_;

public:
	//used in setup
	std::vector<int> inputPresyns_;
	std::vector<int> outputPresyns_;

	//used during collective functions
    std::vector<SpikeItem> sendBuf;
    std::vector<SpikeItem> recvBuf;
    std::vector<int> sizeBuf;

	/** \fn MpiSpikeGraph(int n, int rank, size_t numOut, size_t numIn)
	    \brief initializes an MpiSpikeGraph with input params
	    \param n the number of MPI processes
	    \param rank the rank of this processes
	    \param numOut the number of outputPresyns per process
	    \param numIn the maximum number of InputPresyns per node
	 */
	MpiSpikeGraph(int n,int rank,size_t numOut,size_t numIn,size_t eventsPer,MPI_Datatype& dtype) :
		num_procs_(n), rank_(rank), total_received_(0), total_relevent_(0), num_out_(numOut),
		num_in_(numIn), events_per_(eventsPer), mpi_spikeItem_(dtype) {srand(time(NULL)+rank_);}

	/** \fn setup()
	    \brief sets up the environment to prepare for spike exchange.
	    \postcond each process fills in their inputPresyns_ and outputPresyns_ vectors
	 */
	void setup();

	/** \fn create_spike()
	    \returns a new SpikeItem
		\postcond the new spikeItem's dst_ field is set to one of my outputPresyns
	 */
	SpikeItem create_spike();

	/** \fn allgather()
	    \brief performs an allgather operation for sendBuf sizes
	    \postcond sendBuf is filled with spikes
	    \postcond sizeBuf is filled with ints
	    \postcond sendBuf.size() is equal to num_procs_
	    \postcond sizeBuf.size() is equal to num_procs_
	 */
	void allgather();

	/** \fn allgatherv()
	    \brief performs an allgatherv operation for SpikeItems
	    \postcond recvBuf is filled with spikes
	 */
	void allgatherv();

	/** \fn matches(sitem)
	    \brief checks to see if an input SpikeItem was sent to one of my input presyns
	     and if so, delivers the spike to that input presyn
	 */
	bool matches(const SpikeItem &sitem);

	/** \fn reduce_stats()
	    \brief reduces statistics (total received, total relevent) to the root process
	 */
	void reduce_stats();
};

class DistributedSpikeGraph : public MpiSpikeGraph {
private:
  	MPI_Comm neighborhood_;

public:
	std::vector<int> inNeighbors_;
	std::vector<int> outNeighbors_;
	/** \fn DistributedSpikeGraph(int n, int rank, size_t numOut, size_t numIn)
	    \brief type of MpiSpikeGraph where each node is connected to a subset of the
	    entire graph (a neighborhood)
	    \param n the number of MPI processes
	    \param rank the rank of this processes
	    \param numOut the number of outputPresyns per process
	    \param numIn the number of inputPresyns per process
	 */
	DistributedSpikeGraph(int n, int rank, size_t numOut, size_t numIn,
		size_t eventsPer, MPI_Datatype dtype) :
		MpiSpikeGraph(n, rank, numOut, numIn, eventsPer, dtype) {}

	/** \fn setup()
	    \brief creates the local neighborhood for this process, to set up a distributed
	    graph topology.
	 */
	void setup();

	/** \fn allgather()
	    \brief performs an allgather operation for sendBuf sizes
	    \postcond sendBuf is filled with spikes
	    \postcond sizeBuf is filled with ints
	    \postcond sendBuf.size() is equal to num_procs_
	    \postcond sizeBuf.size() is equal to num_procs_
	 */
	void allgather();

	/** \fn allgatherv()
	    \brief performs and allgatherv operation for SpikeItems
	    \postcond recvBuf is filled with spikes
	 */
	void allgatherv();
};

/** \fn createMpiItemType()
    \brief creates the type mpi_spikeItem in order to send SpikeItems using MPI
 */
MPI_Datatype createMpiItemType(MPI_Datatype dt);

#endif
