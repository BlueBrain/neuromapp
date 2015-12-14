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

#include <omp.h>
#include "queueing/container.h"
#include "queueing/thread.h"

#ifndef pool_h
#define pool_h

class NrnThreadData;

class Pool {
private:
	int numThreads_;
	int time_;
	int eventsPerStep_;
	int all_sent_;
	int all_enqueued_;

	NrnThreadData** threadDatas;
	/// Vector for inter thread events
	std::vector<Event*>* interThreadEvents;
	omp_lock_t* mut_;

public:
	/** \fn Pool(int nt,int verbose, int eventsPer)
	    \brief Initializes a pool with an interThreadEvents and threadDatas array
	    \param nt number of threads
	    \param verbose verbose mode: 1 = ON, 0 = OFF
	    \eventsPer number of events per time step
	 */
	Pool(int,int,int);

	/** \fn ~Pool()
	    \brief destroys interThreadEvents and threadDatas. Frees mutex.
	 */
	~Pool();

/***********************
MULTI_THREADED FUNCTIONS
************************/

	/** \fn void checkThresh(int totalTime)
	    \brief Every thread generates events which are sent to random destination threads
	    \param totalTime tells the provides the total simulation time
	 */
	void checkThresh(int);

	/** \fn void enqueueAll()
	    \brief every thread pushes its interThreadEvents arrays onto its priority queue
	 */
	void enqueueAll();

	/** \fn void deliverAll()
	    \brief every thread pops events (with time up to current time)
	    from their queue and delivers the events
	 */
	void deliverAll();


/***************************
INTERTHREAD EVENTS ACCESSORS
****************************/
	/** \fn void push(Event* it,int dst)
	    \brief pushes event onto the it-events array of dst. Increments all_sent.
	    \param it the item to be pushed
	    \param dst the destination thread
	 */
	inline void push(Event* it, int dst){all_sent_++;interThreadEvents[dst].push_back(it);}

	/** \fn Event* pull(int id,int it)
	    \brief accesses interThreadEvents[id][it]. Increments all_enqueued.
	    \param id specifies interThreadEvents array to access
	    \param it the index of the array to be accessed
	 */
	inline Event* pull(int id, int it){all_enqueued_++;return interThreadEvents[id][it];}

	/** \fn int size(int id)
	    \brief accesses interThreadEvents[id].size()
	    \param id specifies interThreadEvents array to access
	    \return accessed element
	 */
	inline int size(int id){return interThreadEvents[id].size();}

	/** \fn void clear(int)
	    \brief clears the specified interThreadEvents array
	    \param id specifies which array to clear
	    \return array size
	 */
	inline void clear(int id){interThreadEvents[id].clear();}

	/** \fn void lock()
	    \brief aquire mutex
	 */
	inline void lock(){if (mut_) {omp_set_lock(mut_);}}

	/** \fn void unlock()
	    \brief release mutex
	 */
	inline void unlock(){if (mut_) {omp_unset_lock(mut_);}}
};

#endif
