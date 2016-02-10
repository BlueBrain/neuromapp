#include <iostream>
#include <stdlib.h>
#include <stddef.h>
#include <boost/range/algorithm/random_shuffle.hpp>

#include "spike/environment.h"

namespace spike {

environment::environment(int ev, int out, int in, int st, int procs, int rank){
    events_per_ = ev;
    num_out_ = out;
    num_in_ = in;
    sim_time_ = st;
    num_procs_ = procs;
    rank_ = rank;
    spike_item sitem;

    //assign input and output gid's
    for(int i = 0; i < (num_procs_ * num_out_); ++i){
        if(i >= (rank_ * num_out_) && i < ((rank_ * num_out_) + num_out_)){
	    output_presyns_.push_back(i);
	}
	else{
	    input_presyns_.push_back(i);
	}
    }
    assert(input_presyns_.size() >= num_in_);
    boost::random_shuffle(input_presyns_);
    input_presyns_.resize(num_in_);

    int index;
    //generate all spikes that will be sent during the simulation
    for(int i = 0; i < (events_per_ * sim_time_); ++i){
        index = rand() % output_presyns_.size();
        sitem.dst_ = output_presyns_[index];
        sitem.t_ = (rand() / (double)RAND_MAX);
        generated_spikes_.push_back(sitem);
    }

    spikein_.resize(num_procs_ * events_per_ * min_delay_);
    nin_.resize(num_procs_);
    displ_.resize(num_procs_);
}

void environment::load_spikeout(){
    spikeout_.clear();
    for(int i = 0; i < (events_per_ * min_delay_); ++i){
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
}

bool environment::matches(const spike_item &sitem){
    for(int i = 0; i < input_presyns_.size(); ++i){
        if(sitem.dst_ == input_presyns_[i]){
            return true;
        }
    }
    return false;
}

}


