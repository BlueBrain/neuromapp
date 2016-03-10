/*
 * Neuromapp - evironment.cpp, Copyright (c), 2015,
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
 * @file neuromapp/spike/environment.cpp
 * contains definitions for dummy environment class
 */
#include <iostream>
#include <stdlib.h>
#include <stddef.h>
#include <vector>
#include <unistd.h>

#include "spike/environment.h"

namespace spike {

environment::environment(int nspikes,int simtime,int out,int in,int netcons,int nprocs,int rank){
    num_out_ = out;
    num_in_ = in;
    netcons_per_input_ = netcons;
    num_procs_ = nprocs;
    rank_ = rank;
    total_received_ = 0;
    total_relevant_ = 0;
    num_spikes_ = nspikes;
    sim_time_ = simtime;
    time_ = 0;

    //random number generation for setup and spike vals spikes
    int seed = time(NULL)+rank;
    exp_dist_lambda_ = static_cast<double>(num_spikes_) /
                       static_cast<double>(sim_time_ - min_delay_);
    std::cout<<"lambda = "<<exp_dist_lambda_<<std::endl;
    rng_ = random_gen(seed, (out - 1), exp_dist_lambda_);

    //assign input and output gid's
    int_vec available_inputs;
    int_vec cellgroups;
    if(num_procs_ > 1){
        for(int i = 0; i < (num_procs_ * num_out_); ++i){
            if(i >= (rank_ * num_out_) && i < ((rank_ * num_out_) + num_out_)){
	        output_presyns_.push_back(i);
            }
            else{
                available_inputs.push_back(i);
            }
        }
        //create a randomly ordered list of input_presyns_
        assert(available_inputs.size() >= num_in_);
        boost::random_shuffle(available_inputs);
        available_inputs.resize(num_in_);

        //create a vector of randomly ordered cellgroups
        cellgroups.resize(num_cells_);
        boost::iota(cellgroups, 0);

        boost::random_shuffle(cellgroups);

        //for each input presyn,
        //select N unique netcons to cell groups
        for(int i = 0; i < num_in_; ++i){
            int presyn = available_inputs[i];
            for(int j = 0; j < netcons_per_input_; ++j){
                input_presyns_[presyn].push_back(cellgroups[j]);
            }
        }
    }

    else{
        for(int i = 0; i < num_out_; ++i){
	    output_presyns_.push_back(i);
        }
        assert(input_presyns_.empty());
    }
    nin_.resize(num_procs_);
    displ_.resize(num_procs_);
}

void environment::generate_all_events(){
    int index;
    double last_spike = 0.0;
    spike_item sitem;
    //generate all spikes that will be sent during the simulation
    spike_vec::iterator it = generated_spikes_.begin();
    for(int i = 0; i < num_spikes_; ++i){
        index = rng_.gen_uni();
        sitem.data_ = output_presyns_[index];
        last_spike += rng_.gen_exp();
        sitem.t_ = static_cast<int>(last_spike);
        sitem.t_ += min_delay_;
        it = generated_spikes_.insert(it, sitem);
    }
}


void environment::time_step(){
    //enqueue all spikes up to the current time
    while(!generated_spikes_.empty() && generated_spikes_.back().t_ <= time_){
        spikeout_.push_back(generated_spikes_.back());
        generated_spikes_.pop_back();
    }
}

void environment::set_displ(){
    displ_[0] = 0;
    int total = nin_[0];
    for(int i=1; i < num_procs_; ++i){
        displ_[i] = total;
        total += nin_[i];
    }
    spikein_.resize(total);
    std::cout<<"Total spikes sent at time "<<time_<<": "<<total<<std::endl;
    std::cout<<"Outbound spikes: "<<spikeout_.size()<<std::endl;
}

void environment::filter(){
    total_received_ += spikein_.size();
    std::map<int, int_vec>::iterator it;
    spike_item ev;
    int rel = 0;
    int cntr = 0;
    for(size_t i = 0; i < spikein_.size(); ++i){
        it = input_presyns_.begin();
        ev = spikein_[i];
        it = input_presyns_.find(ev.data_);
        if(it != input_presyns_.end()){
            ++total_relevant_;
        }
        else{
        }
    }
}

//USED BY NON-BLOCKING API
void environment::parallel_send(){
    //should still send out events
    //all other tasks are calls to usleep
    //enqueue all spikes up to the current time
    while(!generated_spikes_.empty() && generated_spikes_.back().t_ <= time_){
        spikeout_.push_back(generated_spikes_.back());
        generated_spikes_.pop_back();
    }
}

}


