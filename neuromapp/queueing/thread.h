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
#include "queueing/container.h"
#include "queueing/pool.h"

class Pool;

class NrnThreadData{
private:
	int id_;
	int v_;
	int sent_;
	int enqueued_;
	Queue *qe_;

public:
	/** \fn NrnThreadData(int i,int verbose)
	    \brief initializes NrnThreadData and creates a new priority queue
	    \param i provides the thread id for this NrnThreadData
	    \param verbose verbose mode: 1 = ON, 0 = OFF
	 */
	NrnThreadData(int,int);

	/** \fn ~NrnThreadData()
	    \brief frees queue and destroys NrnThreadData object
	 */
	~NrnThreadData();

	/** \fn void interThreadSend(double d, double tt, int dst, Pool& p)
	    \brief sends an Event to the destination thread's array
	    \param d the event's data value
	    \param tt the event's time value
	    \param dst the destination thread
	    \param p the Pool containing the interThreadEvents arrays
	 */
	void interThreadSend(double,double,int,Pool&);

	/** \fn void enqeueMyEvents(Pool& p)
	    \brief (for this thread) push all the events from my array to my priority queue
	    \param p the Pool containing the interThreadEvents arrays
	 */
	void enqueueMyEvents(Pool&);

	/** \fn void selfSend(double d, double tt)
	    \brief send an item directly to my priority queue
	    \param d the Event's data value
	    \param tt the Event's time value
	 */
	void selfSend(double,double);

	/** \fn bool deliver(int til)
	    \brief dequeue all items with time < til
	    \param til the current time. compared against event times
	 */
	bool deliver(int);
};

#endif
