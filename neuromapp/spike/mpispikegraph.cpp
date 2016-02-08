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

void MpiSpikeGraph::setup(){

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
    recv_buf_.resize(events_per_ * num_procs_);
}

void MpiSpikeGraph::generate_spikes(){
    //Select a random number of items to generate
    spike_item sitem;
    int index;
    for(int i = 0; i < events_per_; ++i){
        //select the dstProcess
        index = rand() % output_presyns_.size();
        sitem.dst_ = output_presyns_[index];
        sitem.t_ = (rand() / (double)RAND_MAX);
        generated_spikes_.push_back(sitem);
    }
}

void MpiSpikeGraph::allgather(){
    int size = send_buf_.size();
    MPI_Allgather(&size, 1, MPI_INT, &size_buf_[0], 1, MPI_INT, MPI_COMM_WORLD);
}

void MpiSpikeGraph::set_displ(){
    displ_[0] = 0;
    int total = size_buf_[0];
    for(int i=1; i < num_procs_; ++i){
        displ_[i] = total;
        total += size_buf_[i];
    }
    total_received_ += total;
}

void MpiSpikeGraph::allgatherv(){
    MPI_Allgatherv(&(send_buf_[0]), send_buf_.size(), mpi_spike_item_,
    &recv_buf_[0], &(size_buf_[0]), &displ_[0], mpi_spike_item_, MPI_COMM_WORLD);
}

void MpiSpikeGraph::reduce_stats(int received, int relevent){
    //exchange total_received_
    if(rank_ == 0)
        MPI_Reduce(MPI_IN_PLACE, &received, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    else
        MPI_Reduce(&received, &received, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    //exchange total_relevent_
    if(rank_ == 0)
        MPI_Reduce(MPI_IN_PLACE, &relevent, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    else
        MPI_Reduce(&relevent, &relevent, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank_ == 0){
        std::cout<<"Relevant events: "<<relevent<<std::endl;
        std::cout<<"Received events: "<<received<<std::endl;
    }
}
