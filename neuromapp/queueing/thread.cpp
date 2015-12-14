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
#include "queueing/thread.h"

NrnThreadData::NrnThreadData(int i, int verbose){
	id_ = i;
	v_ = verbose;
	qe_ = new Queue();
	sent_ = 0;
	enqueued_ = 0;
}

NrnThreadData::~NrnThreadData(){
	if(v_){
		std::cout<<"Thread "<<id_<<" sent: "<<sent_<<std::endl;
		std::cout<<"Thread "<<id_<<" enqueued: "<<enqueued_<<std::endl;
	}
	delete qe_;
}

void NrnThreadData::interThreadSend(double d, double tt, int dst, Pool& p){
	Event* it = new Event(d,tt);
	sent_++;
	p.lock();
	p.push(it, dst);
	p.unlock();
}

void NrnThreadData::enqueueMyEvents(Pool& p){
	Event* ite;
	p.lock();
	for (int i = 0; i < p.size(id_); ++i){
		ite = p.pull(id_, i);
		enqueued_++;
		selfSend(ite->data_, ite->t_);
	}
	p.clear(id_);
	p.unlock();
}

void NrnThreadData::selfSend(double d, double tt){
	qe_->insert(tt, d);
}

bool NrnThreadData::deliver(int til){
	Event *q;
	if(q = (qe_->atomic_dq(til))){
//		POINT_RECEIVE()
		return true;
	}
	return false;
}
