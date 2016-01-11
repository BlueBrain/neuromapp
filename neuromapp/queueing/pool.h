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
 * @file neuromapp/queueing/pool.h
 * Contains Pool class declaration.
 */

#include "queueing/thread.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#ifndef pool_h
#define pool_h

class NrnThreadData;

class Pool {
private:
	int cell_groups_;
	int time_;
	bool v_;
	int percent_ITE_;
	int events_per_step_;
	int all_ite_received_;
	int all_enqueued_;
	int all_delivered_;
	int all_spiked_;
	const static int min_delay_ = 5;
	int percent_spike_;

	NrnThreadData* threadDatas;

public:
	/** \fn Pool(int verbose, int eventsPer, int percent_ITE_)
	    \brief initializes a Pool with a threadDatas array
	    \param verbose verbose mode: 1 = on, 0 = off
	    \param events_per_step_ number of events per time step
	    \param percent_ITE_ is the percentage of inter-thread events
	    \param isSpike determines whether or not there are spike events
	 */
	Pool(bool,int,int,bool);

	/** \fn ~Pool()
	    \brief destroys threadDatas.
	 */
	~Pool();

	/** \fn void timeStep(int totalTime)
	    \brief master function to call generate, enqueue, and deliver
	    \param totalTime tells the provides the total simulation time
	 */
	void timeStep(int);

	/** \fn void generateEvents(int totalTime, int myID)
	    \brief creates events which are sent to random destination threads
	    \param totalTime tells the provides the total simulation time
	    \param i the thread index
	 */
	void generateEvents(int,int);

	/** \fn void handleSpike(int totalTime)
	    \brief compensates for the spike exchange by adding events every 5 timesteps
	    \param totalTime tells the provides the total simulation time
	 */
	void handleSpike(int);

	/** \fn int chooseDst(int myID)
	    \brief Generates a random destination according to the variable percent_ITE_
	    \param myID the thread index
	    \return destination
	 */
	int chooseDst(int);
};

#endif
