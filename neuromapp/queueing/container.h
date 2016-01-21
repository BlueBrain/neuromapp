/*
 * Neuromapp - container.h, Copyright (c), 2015,
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
 * @file neuromapp/queueing/container.h
 * \brief Contains Queue and Event class declaration
 */

#include <stdio.h>
#include <assert.h>
#include <queue>
#include <vector>
#include <map>
#include <utility>

#ifndef MAPP_CONTAINER_H_
#define MAPP_CONTAINER_H_

struct Event {
	/** \fn Event(double,double)
	    \brief Initializes an event with data d and time t
	    \param d event data
	    \param t event time
	 */
	explicit Event(double d=double(), double t=double(), bool s=false):data_(d),t_(t),set_(s){};
	double data_;
	double t_;
    bool set_;
};

class Queue {
public:
	typedef std::pair<double, Event> QPair;

	struct less_time{
	    bool operator() (const QPair &x, const QPair &y) const {
				return x.first > y.first;
		}
	};

	/** \fn remove_negative()
	    \brief removes all negative-time events from the queue
	 */
	void remove_negative();

	/** \fn valid_time()
	    \brief checks whether the priority queue top item can be popped
		\return true if criteria is met else false
	 */
	bool valid_time(double tt);

	/** \fn Event* atomic_dq(double til)
	    \brief pops a single event off of the queue with time < til
	    \param til a double value compared against top time.
		\return the popped item
	 */
	Event atomic_dq(double til);

	/** \fn void insert(double t, double data)
	    \brief inserts an event with time t and data value
	    \param t the event time.
	    \param data the event data.
	 */
	void insert(double t, double data);

	/** \fn make_QPair
	 *  \brief creates a QPair from an event
	 */
	QPair make_QPair(Event p) { return QPair(p.t_,p); }

private:
	std::priority_queue<QPair, std::vector<QPair>, less_time> pq_que;
};

#endif
