/*
 * Neuromapp - pool.cpp, Copyright (c), 2015,
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
 * @file neuromapp/queueing/pool.cpp
 * Contains Pool class implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <ctime>
#include "queueing/pool.h"
#include "queueing/thread.h"

const int min_delay = 5;
const int percent_spike = 3;

Pool::Pool(int verbose, int eventsPer, int pITE){
    events_per_step_ = eventsPer;
    percent_ITE_ = pITE;
    v_ = verbose;
    cell_groups_ = 8;
    time_ = 0;
    all_sent_ = 0;
    all_enqueued_ = 0;
    all_delivered_ = 0;
    all_spiked_ = 0;

    //Generate ThreadData array
    threadDatas = new NrnThreadData[cell_groups_];
}

Pool::~Pool(){
    for(int i=0; i < cell_groups_; ++i){
	all_sent_ += threadDatas[i].sent_;
	all_enqueued_ += threadDatas[i].enqueued_;
	all_delivered_ += threadDatas[i].delivered_;
	if(v_){
	    std::cout<<"Thread "<<i<<" sent: "<<threadDatas[i].sent_<<std::endl;
	    std::cout<<"Thread "<<i<<" enqueued: "<<threadDatas[i].enqueued_<<std::endl;
	    std::cout<<"Thread "<<i<<" delivered: "<<threadDatas[i].delivered_<<std::endl;
	}
    }

    delete [] threadDatas;
    if(v_){
    	std::cout<<"Total sent: "<<all_sent_<<std::endl;
    	std::cout<<"Total enqueued: "<<all_enqueued_<<std::endl;
    	std::cout<<"Total delivered: "<<all_delivered_<<std::endl;
    	std::cout<<"Total spiked: "<<all_spiked_<<std::endl;
    }
}

void Pool::timeStep(int totalTime){
#pragma omp parallel for schedule(static,1)
    for(int i=0; i < cell_groups_; ++i){
	generateEvents(totalTime,i);
    	threadDatas[i].enqueueMyEvents(); //Have threads enqueue their interThreadEvents
    	while(threadDatas[i].deliver(i, time_)); // deliver
    }
    time_++;
}

void Pool::handleSpike(int totalTime){
    int max_dt = totalTime/10;
    if( (time_ % ::min_delay) == 0){
	//serial distribution of spike events to inter-thread events
	int num_spikes = ::min_delay * cell_groups_ * events_per_step_ * ::percent_spike / 100;
	int dst;
	double data, tt;
	for(int i = 0; i < num_spikes; ++i){
	   tt = (double)(time_ + (rand() % max_dt) + ::min_delay);
	   dst = rand() % cell_groups_;
	   data = (double)dst;
	   threadDatas[dst].selfSend(data, tt);
	   all_spiked_++;
	}
    }
}

void Pool::generateEvents(int totalTime, int myID){
    //Generate threads, have each thread check their netcons
    //and send events to destination threads.
    int max_dt = totalTime/10;
    /// Simulated target of a NetCon and the event time
    double tt;
    double data = 0;
    int dst_nt;
    srand(time(NULL));
    for(int j=0; j < events_per_step_; ++j){
	//set time_ to be some time in the future t + dt
	tt = (double)(time_ + (rand() % max_dt));
	dst_nt = chooseDst(myID);
	data = (double)dst_nt;
	if (dst_nt == myID) {
	    threadDatas[dst_nt].selfSend(data, tt);
	}
	else {
	    threadDatas[dst_nt].interThreadSend(data, tt + ::min_delay);
	}
    }
}

int Pool::chooseDst(int myID){
    int dst = myID;
    if ((rand() % 100) < percent_ITE_){
    	//destination is another thread
	while(dst == myID){
	    dst = rand() % cell_groups_;
	}
    }
    return dst;
}
