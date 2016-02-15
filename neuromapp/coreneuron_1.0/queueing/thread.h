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

#include <queue>
#include <vector>
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <utility>

#include "coreneuron_1.0/kernel/mechanism/mechanism.h"
#include "coreneuron_1.0/kernel/helper.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/solver/hines.h"
#include "utils/storage/storage.h"

#include "coreneuron_1.0/queueing/queue.h"
#include "coreneuron_1.0/queueing/spinlock_queue.h"
#include "coreneuron_1.0/queueing/lock.h"
#include "coreneuron_1.0/common/data/helper.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace queueing {
typedef std::pair<event,bool> gen_event;

enum implementation {mutex, spinlock};

template<implementation I>
struct inter_thread;

template<>
struct inter_thread<mutex>{
    /// vector for inter thread events
    spinlock_queue<event> inter_thread_events_;
    std::vector<event> q_;
#ifdef _OPENMP
    omp_lock lock_;
#else
    dummy_lock lock_;
#endif
};

template<>
struct inter_thread<spinlock>{
    /// linked-list for inter thread events
    spinlock_queue<event> q_;
};

template<implementation I>
class nrn_thread_data{
private:
    queue qe_;
    NrnThread* nt_;
    inter_thread<I> inter_thread_events_;
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
    nrn_thread_data(): ite_received_(0), enqueued_(0), delivered_(0) {
        input_parameters p;
        char name[] = "coreneuron_1.0_queueing_data";
        std::string data = mapp::data_test();
        p.name = name;

        std::vector<char> chardata(data.begin(), data.end());
        chardata.push_back('\0');
        p.d = &chardata[0];
        nt_ = (NrnThread *) storage_get(p.name, make_nrnthread, p.d, free_nrnthread);
        if(nt_ == NULL){
            std::cerr<<"Error: Unable to open data file"<<std::endl;
            storage_clear(p.name);
            exit(EXIT_FAILURE);
        }
    }

    /** \fn void self_send(int d, double tt)
     *  \brief send an item directly to my priority queue
     *  \param d the Event's data value
     *  \param tt the Event's time value
     **/
    void self_send(int d, double tt){
        enqueued_++;
        qe_.insert(tt, d);
    }

    /** \fn void l_algebra()
     *  \brief performs the mechanism calculations/updates for linear algebra
     */
    void l_algebra(int time){
        nt_->_t = static_cast<double>(time);

           //Update the current
           mech_current_NaTs2_t(nt_,&(nt_->ml[17]));
           mech_current_Ih(nt_,&(nt_->ml[10]));
           mech_current_ProbAMPANMDA_EMS(nt_,&(nt_->ml[18]));

        //Call solver
        nrn_solve_minimal(nt_);

        //Update the states
        mech_state_NaTs2_t(nt_,&(nt_->ml[17]));
        mech_state_Ih(nt_,&(nt_->ml[10]));
        mech_state_ProbAMPANMDA_EMS(nt_,&(nt_->ml[18]));
    }

    /** \fn bool deliver(int id, int til)
     *  \brief dequeue all items with time < til
     *  \param id used in sanity check to verify destination
     *  \param til the current time. compared against event times
     *  \return true if event delivered, else false
     */
    bool deliver(int id, int til){
        event q;
        if(qe_.atomic_dq(til,q)){
            delivered_++;
            assert(q.data_ == id);

            // Use imitation of the point_receive of CoreNeron.
            // Varies per a specific simulation case.
            // Uses reduced version of net_receive of ProbAMPANMDA mechanism.
            mech_net_receive(nt_,&(nt_->ml[18]));
            return true;
        }
        return false;
    }

    /** \fn inter_thread_size()
     *  \return the size of inter_thread_events_
     */
    size_t inter_thread_size(){return inter_thread_events_.q_.size();}

    /** \fn pq_size()
     *  \return the size of qe_
     */
    size_t pq_size(){return qe_.size();}

    /** \fn push_generated_event(double d, double tt, bool s)
     *  \brief pushes an event into generated event vector
     */
    void push_generated_event(double d, double tt, bool s){
        event e;
        e.data_ = d;
        e.t_ = tt;
        gen_event g(e, s);
        generated_events_.push_back(g);
    }

    /** \fn pop_generated_event()
     *  \brief pops a generated event
     *  \returns the next event in generated event vector
     */
    gen_event pop_generated_event(){
        assert(generated_events_.size() > 0);
        gen_event e = generated_events_.back();
        generated_events_.pop_back();
        return e;
    }

    /** \fn void inter_thread_send(double d, double tt)
     *  \brief sends an Event to the destination thread's array
     *  \param d the event's data value
     *  \param tt the event's time value
     */
    void inter_thread_send(double d, double tt) {}

    /** \fn void enqeue_my_events()
     *  \brief (for this thread) push all the events from my
     *  ites_ to my priority queue
     */
    void enqueue_my_events() {}
};

template<>
inline void nrn_thread_data<mutex>::inter_thread_send(double d, double tt){
    inter_thread_events_.lock_.acquire();
    event ite;
    ite.data_ = d;
    ite.t_ = tt;
    ite_received_++;
    inter_thread_events_.q_.push_back(ite);
    inter_thread_events_.lock_.release();
}

template<>
inline void nrn_thread_data<mutex>::enqueue_my_events(){
    inter_thread_events_.lock_.acquire();
    event ite;
    for(int i = 0; i < inter_thread_events_.q_.size(); ++i){
        ite = inter_thread_events_.q_[i];
        self_send(ite.data_, ite.t_);
    }
    inter_thread_events_.q_.clear();
    inter_thread_events_.lock_.release();
}

template<>
inline void nrn_thread_data<spinlock>::inter_thread_send(double d, double tt){
    event ite;
    ite.data_ = d;
    ite.t_ = tt;
    inter_thread_events_.q_.push(ite, ite_received_);
}

template<>
inline void nrn_thread_data<spinlock>::enqueue_my_events(){
    spinlock_queue<event>::node* head = inter_thread_events_.q_.pop_all();
    spinlock_queue<event>::node* elem = NULL;
    event ite;
    while(head){
        elem = head;
        ite = elem->data;
        self_send(ite.data_, ite.t_);
        head = head->next;
        delete elem;
    }
}

} //endnamespace
#endif
