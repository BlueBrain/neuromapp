/*
 * Neuromapp - pool.ipp, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/event_passing/queueing/pool.ipp
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

template<typename G, typename P>
void pool::send_events(int myID, G& generator, P& presyns){
    int curTime = thread_datas_[myID].get_time();
    int gid = 0;
    int dest;
    const environment::presyn* output = NULL;
    event new_event;
    try{
        while(generator.compare_top_lte(myID, curTime)){
            environment::gen_event g = generator.pop(myID);
            gid = g.first;
            output = presyns.find_output(gid);
            if(output == NULL){
                std::cout<<"Rank: "<<rank_<<" Could not find gid: "<<gid<<std::endl;
                assert(false);
            }
            //send to all local destinations
            for(int i = 0; i < output->size(); ++i){
                dest = (*output)[i] % thread_datas_.size();
                if(dest == myID)
                    thread_datas_[myID].self_send(gid, g.second);
                else
                    thread_datas_[dest].inter_thread_send(gid, g.second);
            }
            //send to spikeout_ buffer
            new_event.data_ = gid;
            new_event.t_ = g.second;

            spike_.lock_.acquire();
            spike_.spikeout_.push_back(new_event);
            ++(spike_.spike_stats_);
            spike_.lock_.release();
        }
    }
    catch(const std::bad_alloc& e) {
        std::cout <<"send failed: "<<e.what()<<std::endl;
    }
}

//PARALLEL FUNCTIONS
template <typename G, typename P>
void pool::fixed_step(G& generator, P& presyns){
    #pragma omp parallel for schedule(static,1)
    for(int i = 0; i < thread_datas_.size(); ++i){
        for(int j = 0; j < min_delay_; ++j){
            send_events(i, generator, presyns);
            //Have threads enqueue their interThreadEvents
            thread_datas_[i].enqueue_my_events();

            if(perform_algebra_)
                thread_datas_[i].l_algebra();

            /// Deliver events
            while(thread_datas_[i].deliver());

            thread_datas_[i].increment_time();
        }
    }
    time_ += min_delay_;
}

template <typename P>
void pool::filter(P& presyns){
    std::map<int, std::vector<int> >::iterator it;
    double tt;
    const environment::presyn* input = NULL;
    int spike_gid;
    int dest;
    try{
        spike_.received_spike_stats_ += spike_.spikein_.size();
        for(int i = 0; i < spike_.spikein_.size(); ++i){
            tt = spike_.spikein_[i].t_;
            spike_gid = spike_.spikein_[i].data_;
            if((input = presyns.find_input(spike_gid)) != NULL){
                for(size_t j = 0; j < input->size(); ++j){
                    dest = (*input)[j] % thread_datas_.size();
                    //send using non-mutex inter-thread send here
                    thread_datas_[dest].inter_send_no_lock(dest, tt);
                    ++(spike_.post_spike_stats_);
                }
            }
        }
    }
    catch(const std::bad_alloc& e) {
        std::cout<<"Rank: "<<rank_<<" failed receiving: "<<spike_gid<<std::endl;
        std::cout <<"Filter failed: "<<e.what()<<std::endl;
    }

    spike_.spikeout_.clear();
    spike_.spikein_.clear();
}

inline void pool::accumulate_stats(){
    int ite_stats = 0;
    int local_stats = 0;
    for(int i=0; i < thread_datas_.size(); ++i){
        ite_stats += thread_datas_[i].ite_received_;
        local_stats += thread_datas_[i].local_received_;
        assert(thread_datas_[i].get_time() == time_);
    }

    //ACCUMULATE ACROSS RANKS
    spike_.ite_stats_ = ite_stats;
    spike_.local_stats_ = local_stats;
}

} //end of namespace

#endif
