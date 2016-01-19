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
	MPI_Datatype mpi_spikeItem;
	int num_procs_;
	int rank_;
	size_t events_per_;
	size_t num_out_;
	size_t num_in_;
	std::vector<int> inputPresyns_;
	std::vector<int> outputPresyns_;

public:
	/** \fn MpiSpikeGraph(int n, int rank, size_t numOut, size_t numIn)
	    \brief initializes an MpiSpikeGraph with input params
	    \param n the number of MPI processes
	    \param rank the rank of this processes
	    \param numOut the number of outputPresyns per process
	    \param numIn the maximum number of InputPresyns per node
	 */
	MpiSpikeGraph(int n, int rank, size_t numOut, size_t numIn) :
	num_procs_(n), rank_(rank), num_out_(numOut), num_in_(numIn) {srand(time(NULL)+rank_);}

	/** \fn ~MpiSpike()
	    \brief delete MpiSpike fields
	 */
	virtual ~MpiSpikeGraph();

	/** \fn setup()
	    \brief sets up the environment to prepare for spike exchange.
	    \postcond each process fills in their inputPresyns_ and outputPresyns_ vectors
	 */
	void setup();

	/** \fn createMpiItemType()
	    \brief creates the type mpi_spikeItem in order to send SpikeItems using MPI
	 */
	void createMpiItemType();

	/** \fn freeMPIItemType()
	    \brief frees up the mpi_spikeItem type (called before MPI_Finalize)
	 */
	void freeMpiItemType();

	/** \fn create_spike()
	    \brief generates a new SpikeItem
	 */
	SpikeItem create_spike();

	/** \fn allgather()
	    \brief performs an allgather operation for ints
	 */
	void allgather(int send, std::vector<int> &recv);

	/** \fn allgatherv(send, sizeBuf, recv)
	    \brief performs an allgatherv operation for SpikeItems
	 */
	void allgatherv(std::vector<SpikeItem> &send,
	std::vector<int> &sizeBuf, std::vector<SpikeItem> &recv);

	/** \fn matches(sitem)
	    \brief checks to see if an input SpikeItem was sent to one of my input presyns
	     and if so, delivers the spike to that input presyn
	 */
	bool matches(SpikeItem &sitem);
};

class DistributedSpikeGraph : public MpiSpikeGraph {
private:
  	MPI_Comm neighborhood_;
	std::vector<int> inNeighbors_;
	std::vector<int> outNeighbors_;

public:
	/** \fn DistributedSpikeGraph(int n, int rank, size_t numOut, size_t numIn)
	    \brief type of MpiSpikeGraph where each node is connected to a subset of the
	    entire graph (a neighborhood)
	    \param n the number of MPI processes
	    \param rank the rank of this processes
	    \param numOut the number of outputPresyns per process
	    \param numIn the number of inputPresyns per process
	 */
	DistributedSpikeGraph(int n, int rank, size_t numOut, size_t numIn) :
		MpiSpikeGraph(n, rank, numOut, numIn) {}

	~DistributedSpikeGraph();

	/** \fn setup()
	    \brief creates the local neighborhood for this process, to set up a distributed
	    graph topology.
	 */
	void setup();

	/** \fn create_spike()
	    \brief generates a new SpikeItem
	 */
	SpikeItem create_spike();

	/** \fn allgather()
	    \brief performs an allgather operation for ints
	 */
	void allgather(int send, std::vector<int> &recv);

	/** \fn allgatherv(send, sizeBuf, recv)
	    \brief performs and allgatherv operation for SpikeItems
	 */
	void allgatherv(std::vector<SpikeItem> &send,
	std::vector<int> &sizeBuf, std::vector<SpikeItem> &recv);

	/** \fn matches(sitem)
	    \brief checks to see if an input SpikeItem was sent to one of my input presyns
	     and if so, delivers the spike to that input presyn
	 */
	bool matches(SpikeItem &sitem);
};

#endif
