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

MpiSpikeGraph::~MpiSpikeGraph(){
    inputPresyns_.clear();
    outputPresyns_.clear();
}

void MpiSpikeGraph::setup(){
    createMpiItemType();

    //my inputPresyn's are chosen random with the following conditions:
    //a new inputPresyn cannot be the same as my outputPresyn or an inputPresyn I've already chosen
    //std::vector<int> availableInputs;
    for(int i = 0; i < (num_procs_ * num_out_); ++i){
	//if in range, make gid one of my outputPresyns
        if(i >= (rank_ * num_out_) && i < ((rank_ * num_out_) + num_out_)){
	    outputPresyns_.push_back(i);
	}
	//otherwise gid is available as an input
	else{
	    inputPresyns_.push_back(i);
	}
    }
    assert(inputPresyns_.size() == ((num_procs_ * num_out_) - num_out_));
    assert(outputPresyns_.size() == num_out_);

    //size_t index;
    //size_t tail = availableInputs.size() - 1;
    //random cyclically select input presyns
    boost::random_shuffle(inputPresyns_);
    inputPresyns_.resize(num_in_);
    assert(inputPresyns_.size() == num_in_);
    /*for(int i = 0; i < num_in_; ++i){
	assert(!availableInputs.empty());
	index = rand() % availableInputs.size();
	inputPresyns_.push_back(availableInputs[index]);
	availableInputs[index] = availableInputs[tail];
	availableInputs.pop_back();
	--tail;
    }*/
}

void MpiSpikeGraph::createMpiItemType(){
    const int nblocks = 2;
    int blocklengths[2] = {1,1};
    MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};
    MPI_Aint offsets[2];

    offsets[0] = offsetof(SpikeItem, dst_);
    offsets[1] = offsetof(SpikeItem, t_);

    MPI_Type_create_struct(nblocks, blocklengths, offsets, types, &mpi_spikeItem);
    MPI_Type_commit(&mpi_spikeItem);
}

void MpiSpikeGraph::freeMpiItemType(){
    MPI_Type_free(&mpi_spikeItem);
}

SpikeItem MpiSpikeGraph::create_spike(){
    //Select a random number of items to generate
    SpikeItem sitem;
    //select the dstProcess
    int index = rand() % outputPresyns_.size();
    sitem.dst_ = outputPresyns_[index];
    sitem.t_ = (rand() / (double)RAND_MAX);
}

void MpiSpikeGraph::allgather(int size, std::vector<int> &sizeBuf){
    //gather how many events each neighbor is sending
    MPI_Allgather(&size, 1, MPI_INT, &sizeBuf[0], 1, MPI_INT, MPI_COMM_WORLD);
}

void MpiSpikeGraph::allgatherv(std::vector<SpikeItem> &send,
std::vector<int> &sizeBuf, std::vector<SpikeItem> &recv){
    //get the displacements
    int displ[num_in_];
    displ[0] = 0;
    int totalCnt = sizeBuf[0];
    for(int i=1; i < inputPresyns_.size(); ++i){
	totalCnt += sizeBuf[i];
	displ[i] = displ[i-1] + sizeBuf[i-1];
    }

    //next distribute items to every other process using allgatherv
    MPI_Neighbor_allgatherv(&send[0], send.size(), mpi_spikeItem,
		   &recv[0], &sizeBuf[0], displ, mpi_spikeItem, MPI_COMM_WORLD);

    send.clear();
    sizeBuf.clear();
}


