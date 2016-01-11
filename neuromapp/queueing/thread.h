/*
 * Neuromapp - thread.h, Copyright (c), 2015,
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
 * @file neuromapp/queueing/thread.h
 * Contains NrnThreadData class declaration.
 */

#ifndef thread_h
#define thread_h

#include <queue>
#include <vector>
#include "queueing/container.h"
#include "queueing/lockless_queue.h"

#ifdef _OPENMP
#include <omp.h>
#endif

class NrnThreadData{
private:
	Queue *qe_;
	/// vector for inter thread events
	waitfree_queue<Event> inter_thread_events_;

public:
	int ite_received_;
	int enqueued_;
	int delivered_;

	/** \fn NrnThreadData()
	    \brief initializes NrnThreadData and creates a new priority queue
	    \param i provides the thread id for this NrnThreadData
	    \param verbose verbose mode: 1 = ON, 0 = OFF
	 */
	NrnThreadData();

	/** \fn ~NrnThreadData()
	    \brief frees queue and destroys NrnThreadData object
	 */
	~NrnThreadData();

	/** \fn void interThreadSend(double d, double tt)
	    \brief sends an Event to the destination thread's array
	    \param d the event's data value
	    \param tt the event's time value
	 */
	void interThreadSend(double,double);

	/** \fn void enqeueMyEvents()
	    \brief (for this thread) push all the events from my inter_thread_events_ to my priority queue
	 */
	void enqueueMyEvents();

	/** \fn void selfSend(double d, double tt)
	    \brief send an item directly to my priority queue
	    \param d the Event's data value
	    \param tt the Event's time value
	 */
	void selfSend(double,double);

	/** \fn bool deliver(int id, int til)
	    \brief dequeue all items with time < til
	    \param id used in sanity check to verify destination
	    \param til the current time. compared against event times
	 */
	bool deliver(int,int);
};
#endif
