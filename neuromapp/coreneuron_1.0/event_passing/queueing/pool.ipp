/*
 * Neuromapp - pool.cpp, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/event_passing/queueing/pool.cpp
 * \brief Contains pool class definition.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <ctime>

#ifndef MAPP_POOL_IPP_
#define MAPP_POOL_IPP_

namespace queueing {

template<typename G>
void pool::send_events(int myID, G& generator){
    int curTime = thread_datas_[myID].get_time();
    while((!generator.empty(myID)) &&
    (generator.compare_top_lte(myID, curTime))){
        environment::gen_event g = generator.pop(myID);
        event e = g.first;
        environment::event_type type = g.second;
        //if spike event send to spike_out
        switch(type){
            case environment::SPIKE:
                e.t_ += min_delay_;
                spike_.lock_.acquire();
                spike_.spikeout_.push_back(e);
                spike_.lock_.release();
                break;

            case environment::ITE:
                assert(e.data_ < thread_datas_.size());
                thread_datas_[e.data_].inter_thread_send(
                    e.data_, (e.t_ + min_delay_));
                break;

            case environment::LOCAL:
                assert(e.data_ == myID);
                thread_datas_[e.data_].self_send(e.data_, e.t_);
                break;

            default:
                std::cerr<<"error: invalid event type:"<<type<<std::endl;
                exit(EXIT_FAILURE);
        }
    }
}

//PARALLEL FUNCTIONS
template <typename G>
void pool::fixed_step(G& generator){
    #pragma omp parallel for schedule(static,1)
    for(int i = 0; i < thread_datas_.size(); ++i){
        for(int j = 0; j < min_delay_; ++j){
            send_events(i, generator);
            //Have threads enqueue their interThreadEvents
            thread_datas_[i].enqueue_my_events();

            if(perform_algebra_)
                thread_datas_[i].l_algebra();

            /// Deliver events
            while(thread_datas_[i].deliver(i));

            thread_datas_[i].increment_time();
        }
    }
    time_ += min_delay_;
}

template <typename P>
void pool::filter(P& presyns){
    std::map<int, std::vector<int> >::iterator it;
    event ev;
    environment::input_presyn input;
    int spike_gid;
    received_ += spike_.spikein_.size();
    for(int i = 0; i < spike_.spikein_.size(); ++i){
        spike_gid = spike_.spikein_[i].data_;
        if(presyns.find_input(spike_gid, input)){
            ++relevant_;
            for(size_t j = 0; j < input.second.size(); ++j){
                int dest = input.second[j];
                //send using non-mutex inter-thread send here
                thread_datas_[dest].inter_send_no_lock(dest, ev.t_);
            }
        }
    }
    spike_.spikeout_.clear();
    spike_.spikein_.clear();
}

} //end of namespace

#endif
