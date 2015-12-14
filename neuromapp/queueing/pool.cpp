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
#include <omp.h>
#include <time.h>
#include <ctime>
#include <vector>
#include "queueing/pool.h"
#include "queueing/thread.h"
#include "queueing/container.h"

Pool::Pool(int nt, int verbose, int eventsPer){
    eventsPerStep_ = eventsPer;
    numThreads_ = nt;
    time_ = 0;
    all_sent_ = 0;
    all_enqueued_ = 0;

    //Generate ThreadData array
    threadDatas = new NrnThreadData*[numThreads_];
    interThreadEvents = new std::vector<Event*>[numThreads_];
    for(int i=0; i < numThreads_; ++i){
	threadDatas[i] = new NrnThreadData(i, verbose);
    }
    mut_ = new omp_lock_t;
    omp_init_lock(mut_);
}

Pool::~Pool(){
    for(int i=0; i < numThreads_; ++i){
	delete threadDatas[i];
	interThreadEvents[i].clear();
    }
    delete [] threadDatas;
    delete [] interThreadEvents;
    if (mut_){
	omp_destroy_lock(mut_);
	delete mut_;
	mut_ = (omp_lock_t*)0;
    }
    std::cout<<"Total Sent: "<<all_sent_<<std::endl;
    std::cout<<"Total Enqueued: "<<all_enqueued_<<std::endl;
}

/***********************
MULTI_THREADED FUNCTIONS
***********************/

void Pool::checkThresh(int totalTime){
    //Generate threads, have each thread check their netcons
    //and send events to destination threads.
    int max_dt = totalTime/10;
    #pragma omp parallel for schedule(static,1)
    for(int i=0; i < numThreads_; ++i){
	/// Simulated target of a NetCon and the event time
	double tt;
	double data = 0;
	int dst_nt;
	for(int j=0; j < eventsPerStep_; ++j){
	    //set time_ to be some time in the future t + dt
  	    tt = (double)(time_ + (rand() % max_dt));
	    dst_nt = rand() % numThreads_;
	    if (dst_nt == i) {
		threadDatas[i]->selfSend(data, tt);
	    }
	    else {
	    	threadDatas[i]->interThreadSend(data, tt, dst_nt, *this);
	    }
	}
    }
}

void Pool::enqueueAll(){
    //Have threads enqueue their interThreadEvents
    #pragma omp parallel for schedule(static,1)
    for(int i=0; i < numThreads_; ++i){
        threadDatas[i]->enqueueMyEvents(*this);
    }
}

void Pool::deliverAll(){
    #pragma omp parallel for schedule(static,1)
    for(int i=0; i < numThreads_; ++i){
    	while(threadDatas[i]->deliver(time_));
    }
    //move timestep
    time_++;
}
