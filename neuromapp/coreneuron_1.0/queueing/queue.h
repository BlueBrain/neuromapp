/*
 * Neuromapp - queue.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.s
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/queueing/queue.h
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

namespace queueing {

struct event {
    int data_;
    double t_;
};

class queue {
public:
    struct is_more{
        /** \fn operator()
         *  \brief compares the time of two events
         *  \return true if x(time) < y(time), else false
         */
        bool operator() (const event &x, const event &y) const {
                return x.t_ > y.t_;
        }
    };

    /** \fn size()
     *  \return the size of pq_que
     */
    size_t size(){return pq_que.size();}

    /** \fn Event* atomic_dq(double til, event q)
     *  \brief pops a single event off of the queue with time < til
     *  \param til a double value compared against top time.
     *  \param q is assigned to the popped event.
     *  \return true if popped, else false
     */
    bool atomic_dq(double til, event& q);

    /** \fn void insert(double t, int data)
     *  \brief inserts an event with time t and data value
     *  \param t the event time.
     *  \param data the event data.
     */
    void insert(double t, int data);

private:
    std::priority_queue<event, std::vector<event>, is_more> pq_que;
};

}
#endif
