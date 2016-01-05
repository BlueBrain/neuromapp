/*
 * Neuromapp - pool.h, Copyright (c), 2015,
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
 * @file neuromapp/spike/mpispike.h
 * contains declaration for the MpiSpike class
 */


#include <mpi.h>
#include <assert.h>
#include <vector>

#ifndef mpispike_h
#define mpispike_h

class MpiSpike {
private:
	MPI_Datatype mpi_spikeItem;

struct SpikeItem{
	int dst_;
	double t_;
};

	int rank_;
	int num_procs_;
	int events_per_;
	int gids_per_;
	int totalCnt_;
	int totalSent_;
	int recvd_;
	int* sizeBuf_;
	std::vector<SpikeItem> sendBuf_;
	std::vector<SpikeItem> recvBuf_;

public:
	/** \fn MpiSpike(int n, int rank, int meanEventsPerDT, int gidsPer)
	    \brief set up/allocate buffers
	    \param n the number of MPI processes
	    \param rank the rank of this processes
	    \param meanEventsPerDT the average number of events to generate per dt
	    \param gidsPer the number of gids per process
	 */
	MpiSpike(int,int,int,int);

	/** \fn ~MpiSpike()
	    \brief delete MpiSpike fields
	 */
	~MpiSpike();

	/** \fn createMpiItemType()
	    \brief creates the type mpi_spikeItem in order to send spikeItems using MPI
	 */
	void createMpiItemType();

	/** \fn freeMPIItemType()
	    \brief frees up the mpi_spikeItem type (called before MPI_Finalize)
	 */
	void freeMpiItemType();

	/** \fn generateEvents()
	    \brief create all of the events for this MpiSpike to send per dt
	 */
	void generateEvents();


	/** \fn exchangeSizes()
	    \brief exchange the sizes for all send buffers (every mindelay)
	 */
	void exchangeSizes();

	/** \fn exchangeEvents()
	    \brief exchanges the actual events items (every mindelay)
	 */
	void exchangeEvents();

	/** \fn filterEvents()
	    \brief compare events' destinations against own gids. Filter
	    the matching events and send them to the dest gid.
	 */
	void filterEvents();
};

#endif
