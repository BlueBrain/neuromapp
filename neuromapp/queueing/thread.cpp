 /*
 * Neuromapp - thread.cpp, Copyright (c), 2015,
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
 * @file neuromapp/queueing/thread.cpp
 * Contains NrnThreadData implementation.
 */

#include <iostream>
#include <cassert>
#include "queueing/thread.h"

NrnThreadData::NrnThreadData(){
	qe_ = new Queue();
	sent_ = 0;
	enqueued_ = 0;
	delivered_ = 0;
}

NrnThreadData::~NrnThreadData(){
	delete qe_;
}

void NrnThreadData::interThreadSend(double d, double tt){
	Event ite(d,tt);
	sent_++;
	inter_thread_events_.push(ite);
}

void NrnThreadData::enqueueMyEvents(){
	waitfree_queue<Event>::node* x = inter_thread_events_.pop_all();
	Event ite;
	while(x){
		enqueued_++;
		waitfree_queue<Event>::node* tmp = x;
		ite = tmp->data;
		selfSend(ite.data_, ite.t_);
		x = x->next;
		delete tmp;
	}
}

void NrnThreadData::selfSend(double d, double tt){
	qe_->insert(tt, d);
}

bool NrnThreadData::deliver(int id, int til){
	Event *q;
	if(q = (qe_->atomic_dq(til))){
		assert((int)q->data_ == id);
	        delivered_++;
//		POINT_RECEIVE()
		return true;
	}
	return false;
}
