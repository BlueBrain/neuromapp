/*
 * Neuromapp - thread.h, Copyright (c), 2015,
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/queueing/thread.h
 * \brief Contains nrn_thread_data class declaration.
 */

#ifndef thread_h
#define thread_h

#include "coreneuron_1.0/kernel/mechanism/mechanism.h"
#include "coreneuron_1.0/kernel/helper.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/solver/hines.h"
#include "utils/storage/storage.h"

#include "coreneuron_1.0/queueing/queue.h"
#include "coreneuron_1.0/queueing/lock.h"
#include "coreneuron_1.0/common/data/helper.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace queueing {
typedef std::pair<event,bool> gen_event;

class nrn_thread_data{
private:
#ifdef _OPENMP
    omp_lock lock_;
#else
    dummy_lock lock_;
#endif

    queue qe_;
    NrnThread* nt_;
    /// vector for inter thread events
    std::vector<event> inter_thread_events_;
    std::vector<gen_event> generated_events_;
public:
    int ite_received_;
    int enqueued_;
    int delivered_;

    /** \fn nrn_thread_data()
     *  \brief initializes nrn_thread_data and creates a new priority queue
     *  \param i provides the thread id for this nrn_thread_data
     *   \param verbose verbose mode: 1 = ON, 0 = OFF
     */
    nrn_thread_data();

    /** \fn void self_send(int d, double tt)
     *  \brief send an item directly to my priority queue
     *  \param d the Event's data value
     *  \param tt the Event's time value
     **/
    void self_send(int d, double tt);

    /** \fn void l_algebra()
     *  \brief performs the mechanism calculations/updates for linear algebra
     */
    void l_algebra(int time);

    /** \fn bool deliver(int id, int til)
     *  \brief dequeue all items with time < til
     *  \param id used in sanity check to verify destination
     *  \param til the current time. compared against event times
     *  \return true if event delivered, else false
     */
    bool deliver(int id, int til);

    /** \fn inter_thread_size()
     *  \return the size of inter_thread_events_
     */
    size_t inter_thread_size(){return inter_thread_events_.size();}

    /** \fn pq_size()
     *  \return the size of qe_
     */
    size_t pq_size(){return qe_.size();}

    /** \fn push_generated_event(int d, double tt, bool s)
     *  \brief pushes an event into generated event vector
     */
    void push_generated_event(int d, double tt, bool s);

    /** \fn pop_generated_event()
     *  \brief pops a generated event
     *  \returns the next event in generated event vector
     */
    gen_event pop_generated_event();

    /** \fn void inter_thread_send(int d, double tt)
     *  \brief sends an Event to the destination thread's array
     *  \param d the event's data value
     *  \param tt the event's time value
     */
    void inter_thread_send(int d, double tt);

    /** \fn void inter_send_no_lock(int d, double tt)
     *  \brief send an item to inter_thread_events_ (serially)
     *  \param d the Event's data value
     *  \param tt the Event's time value
     **/
    void inter_send_no_lock(int d, double tt);

    /** \fn void enqeue_my_events()
     *  \brief (for this thread) push all the events from my
     *  ites_ to my priority queue
     */
    void enqueue_my_events();
};

} //endnamespace
#endif
