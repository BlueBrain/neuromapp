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
    const environment::presyn* output = NULL;
    event new_event;
    while(generator.compare_top_lte(myID, curTime)){
        environment::gen_event g = generator.pop(myID);
        gid = g.second;
        output = presyns.find_output(gid);
        assert(output != NULL);
        //send to all local destinations
        for(int i = 0; i < output->size(); ++i){
            if((*output)[i] == myID)
                thread_datas_[myID].self_send(gid, g.first);
            else
                thread_datas_[i].inter_thread_send(gid, g.first);
        }
        //send to spikeout_ buffer
        new_event.data_ = gid;
        new_event.t_ = g.first;

        spike_.lock_.acquire();
        spike_.spikeout_.push_back(new_event);
        spike_.lock_.release();

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
    event ev;
    const environment::presyn* input = NULL;
    int spike_gid;
    received_ += spike_.spikein_.size();
    for(int i = 0; i < spike_.spikein_.size(); ++i){
        spike_gid = spike_.spikein_[i].data_;
        if((input = presyns.find_input(spike_gid)) != NULL){
            ++relevant_;
            for(size_t j = 0; j < input->size(); ++j){
                int dest = (*input)[j];
                //send using non-mutex inter-thread send here
                thread_datas_[dest].inter_send_no_lock(dest, ev.t_);
            }
        }
    }
    spike_.spikeout_.clear();
    spike_.spikein_.clear();
}

inline pool::~pool(){
    int ite_received = 0;
    int local_received = 0;
    int all_enqueued = 0;
    int all_delivered = 0;
    for(int i=0; i < thread_datas_.size(); ++i){
        ite_received += thread_datas_[i].ite_received_;
        local_received += thread_datas_[i].local_received_;
        all_enqueued += thread_datas_[i].enqueued_;
        all_delivered += thread_datas_[i].delivered_;
        assert(thread_datas_[i].get_time() == time_);
    }

    if(rank_ == 0){
        std::cout<<"Total inter-thread received: "<<ite_received<<std::endl;
        std::cout<<"Total local received: "<<local_received<<std::endl;
        std::cout<<"Total spikes received: "<<received_<<std::endl;
    }

/*   std::cout<<"Total enqueued: "<<all_enqueued<<std::endl;
    std::cout<<"Total delivered: "<<all_delivered<<std::endl;
    std::cout<<"Total relevent spikes: "<<relevant_<<std::endl;*/
}

} //end of namespace

#endif
