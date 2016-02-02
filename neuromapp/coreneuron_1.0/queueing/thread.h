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
 * @file neuromapp/coreneuron_1.0/queueing/thread.h
 * \brief Contains NrnThreadData class declaration.
 */

#ifndef thread_h
#define thread_h

#include <queue>
#include <vector>
#include <iostream>
#include <cassert>
#include <unistd.h>

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

enum implementation {mutex, spinlock};

template<implementation I>
struct InterThread;

template<>
struct InterThread<mutex>{
	/// vector for inter thread events
	spinlock_queue<event> inter_thread_events_;
	std::vector<event> q_;
#ifdef _OPENMP
	OMPLock lock_;
#else
	DummyLock lock_;
#endif
};

template<>
struct InterThread<spinlock>{
	/// linked-list for inter thread events
	spinlock_queue<event> q_;
};

template<implementation I>
class NrnThreadData{
private:
	queue qe_;
	NrnThread* nt_;
	InterThread<I> inter_thread_events_;
	std::vector<event> generated_events_;
public:
	int ite_received_;
	int enqueued_;
	int delivered_;

	/** \fn NrnThreadData()
	    \brief initializes NrnThreadData and creates a new priority queue
	    \param i provides the thread id for this NrnThreadData
	    \param verbose verbose mode: 1 = ON, 0 = OFF
	 */
	NrnThreadData(): ite_received_(0), enqueued_(0), delivered_(0) {
		input_parameters p;
		char name[] = "coreneuron_1.0_cstep_data";
		std::string data = mapp::data_test();
		p.name = name;

		std::vector<char> chardata(data.begin(), data.end());
		chardata.push_back('\0');
		p.d = &chardata[0];
    	nt_ = (NrnThread *) storage_get(p.name, make_nrnthread, p.d, free_nrnthread);
		if(nt_ == NULL){
			std::cout<<"Error: Unable to open data file"<<std::endl;
			storage_clear(p.name);
			exit(EXIT_FAILURE);
		}
	}

	/** \fn void selfSend(double d, double tt)
	 *  \brief send an item directly to my priority queue
	 *  \param d the Event's data value
	 *  \param tt the Event's time value
	 **/
	void selfSend(double d, double tt){
		enqueued_++;
		qe_.insert(tt, d);
	}

	/** \fn void l_algebra()
	 *  \brief performs the mechanism calculations/updates for linear algebra
	 */
	void l_algebra(){
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
	    \brief dequeue all items with time < til
	    \param id used in sanity check to verify destination
	    \param til the current time. compared against event times
	    \return true if event delivered, else false
	 */
	bool deliver(int id, int til){
		event q;
		if(qe_.atomic_dq(til,q)){
		    delivered_++;
			assert((int)q.data_ == id);

			// Use imitation of the point_receive calculation time (ms).
			// Varies per a specific simulation case.
			//usleep(10);
			return true;
		}
		return false;
	}

	/** \fn interThreadSize()
	 *  \return the size of inter_thread_events_
	 */
	size_t interThreadSize(){return inter_thread_events_.q_.size();}

    /** \fn PQSize()
	 *  \return the size of qe_
	 */
	size_t PQSize(){return qe_.size();}

    void push_generated_event(double d, double tt){
		event e(d, tt, true);
		generated_events_.push_back(e);
	}

    event pop_generated_event(){
		event e = generated_events_.back();
		generated_events_.pop_back();
		return e;
	}

	/** \fn void interThreadSend(double d, double tt)
	    \brief sends an Event to the destination thread's array
	    \param d the event's data value
	    \param tt the event's time value
	 */
	void interThreadSend(double d, double tt) {}

	/** \fn void enqeueMyEvents()
	    \brief (for this thread) push all the events from my
	    ites_ to my priority queue
	 */
	void enqueueMyEvents() {}
};

template<>
inline void NrnThreadData<mutex>::interThreadSend(double d, double tt){
	inter_thread_events_.lock_.acquire();
	event ite(d,tt,true);
	ite_received_++;
	inter_thread_events_.q_.push_back(ite);
	inter_thread_events_.lock_.release();
}

template<>
inline void NrnThreadData<mutex>::enqueueMyEvents(){
	inter_thread_events_.lock_.acquire();
	event ite = event();
	for(int i = 0; i < inter_thread_events_.q_.size(); ++i){
		ite = inter_thread_events_.q_[i];
		selfSend(ite.data_, ite.t_);
	}
	inter_thread_events_.q_.clear();
	inter_thread_events_.lock_.release();
}

template<>
inline void NrnThreadData<spinlock>::interThreadSend(double d, double tt){
	event ite(d,tt,true);
	ite_received_++;
	inter_thread_events_.q_.push(ite);
}

template<>
inline void NrnThreadData<spinlock>::enqueueMyEvents(){
	spinlock_queue<event>::node* head = inter_thread_events_.q_.pop_all();
	spinlock_queue<event>::node* elem = NULL;
	event ite = event();
	while(head){
		elem = head;
		ite = elem->data;
		selfSend(ite.data_, ite.t_);
		head = head->next;
		delete elem;
	}
}

} //endnamespace
#endif
