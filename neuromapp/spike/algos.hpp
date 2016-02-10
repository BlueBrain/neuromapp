/*
 * Neuromapp - algos.hpp, Copyright (c), 2015,
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
 * @file neuromapp/spike/algos.hpp
 * contains algorithm definitions for SpikeExchangeGraph
 */

#include <assert.h>
#include <cstddef>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

#include "spike/spike_exchange.h"
#include "spike/environment.h"

#ifndef algos_h
#define algos_h

void run_sim(spike::spike_exchange &se, spike::environment& env){
    //load spikeout with the spikes to be sent
    env.load_spikeout();
    //gather how many spikes each process is sending
    se.allgather(env.spikeout_.size(), env.nin_);
    //get the displacements
    env.set_displ();
    //next distribute items to every other process using allgatherv
    se.allgatherv(env.spikeout_, env.spikein_, env.nin_, env.displ_);
}

void non_blocking(spike::spike_exchange &se, spike::environment& env){
    int num_tasks = 5;
    MPI_Request request;
    MPI_Request requestv;
    int flag;
    env.load_spikeout();
    //check thresh
    request = se.Iallgather(env.spikeout_.size(), env.nin_, request);
    for(int i = 0; i < num_tasks; ++i){
        //run task;
        usleep(10);
        //do allgatherv only the first time flag is set
        if(!flag){
            flag = se.get_status(request, flag);
            if(flag){
                env.set_displ();
                requestv = se.Iallgatherv(env.spikeout_, env.spikein_, env.nin_, env.displ_, requestv);
            }
        }
    }
    if(!flag){
        wait(request);
        env.set_displ();
        requestv = se.Iallgatherv(env.spikeout_, env.spikein_, env.nin_, env.displ_, requestv);
    }
    se.wait(requestv);
}

#endif
