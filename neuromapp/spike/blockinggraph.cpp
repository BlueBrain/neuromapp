#include <iostream>
#include <stdlib.h>
#include <cstddef>
#include <mpi.h>
#include <cassert>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>

#include "spike/mpispikegraph.h"

namespace spike{

void blocking_global_graph::allgather(int size, int_vec nin){
    MPI_Allgather(&size, 1, MPI_INT, &nin[0], 1, MPI_INT, MPI_COMM_WORLD);
}

void blocking_global_graph::allgatherv(spike_vec spikeout, spike_vec spikein,
int_vec nin, int_vec displ){
    MPI_Allgatherv(&spikeout[0], spikeout.size(), mpi_spike_item_,
    &spikein[0], &nin[0], &displ[0], mpi_spike_item_, MPI_COMM_WORLD);
}

}
/*
void blocking_global_graph::setup(){
    assert(num_procs_ > 1);
    //my inputPresyn's are chosen random with the following conditions:
    //a new inputPresyn cannot be the same as my output_presyns or an inputPresyn I've already chosen
    for(int i = 0; i < (num_procs_ * num_out_); ++i){
        //if in range, make gid one of my output_presyns
        if(i >= (rank_ * num_out_) && i < ((rank_ * num_out_) + num_out_)){
	    output_presyns_.push_back(i);
	}
	//otherwise gid is available as an input
	else{
	    input_presyns_.push_back(i);
	}
    }
    assert(input_presyns_.size() == ((num_procs_ * num_out_) - num_out_));

    //randomly shuffle input presyns and keep the first num_in_ elements
    boost::random_shuffle(input_presyns_);
    input_presyns_.resize(num_in_);
    size_buf_.resize(num_procs_);
    displ_.resize(num_procs_);
    recv_buf_.resize(events_per_ * num_procs_ * min_delay_);

    generate_spikes();
}

void blocking_global_graph::generate_spikes(){
    //Select a random number of items to generate
    spike_item sitem;
    int index;
    for(int i = 0; i < (events_per_ * sim_time_); ++i){
        //select the dstProcess
        index = rand() % output_presyns_.size();
        std::cout<<"process "<<rank_<<" generating spike for presyn: "<<output_presyns_[index]<<std::endl;
        sitem.dst_ = output_presyns_[index];
        sitem.t_ = (rand() / (double)RAND_MAX);
        generated_spikes_.push_back(sitem);
    }
}

void blocking_global_graph::load_send_buf(){
    send_buf_.clear();
    for(int i = 0; i < (events_per_ * min_delay_); ++i){
        send_buf_.push_back(generated_spikes_.back());
        generated_spikes_.pop_back();
    }
}

void blocking_global_graph::reduce_stats(){
    //exchange total_received_
    if(rank_ == 0)
        MPI_Reduce(MPI_IN_PLACE, &total_received_, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    else
        MPI_Reduce(&total_received_, &total_received_, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    //exchange total_relevent_
    if(rank_ == 0)
        MPI_Reduce(MPI_IN_PLACE, &total_relevent_, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    else
        MPI_Reduce(&total_relevent_, &total_relevent_, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank_ == 0){
        std::cout<<"Relevant events: "<<total_relevent_<<std::endl;
        std::cout<<"Received events: "<<total_received_<<std::endl;
    }
}

bool blocking_global_graph::matches(const spike_item &sitem){
    for(int i = 0; i < input_presyns_.size(); ++i){
        if(sitem.dst_ == input_presyns_[i]){
            ++total_relevent_;
            return true;
        }
    }
    return false;
}
*/
