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
    //a new inputPresyn cannot be the same as my outputPresyns or an inputPresyn I've already chosen
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

	//randomly shuffle input presyns and keep the first num_in_ elements
    boost::random_shuffle(inputPresyns_);
    inputPresyns_.resize(num_in_);
}

SpikeItem MpiSpikeGraph::create_spike(){
    //Select a random number of items to generate
    SpikeItem sitem;
    //select the dstProcess
    int index = rand() % outputPresyns_.size();
    sitem.dst_ = outputPresyns_[index];
    sitem.t_ = (rand() / (double)RAND_MAX);
	return sitem;
}

void MpiSpikeGraph::allgather(){
	SpikeItem sitem;
    sendBuf.clear();
	for(int i = 0; i < events_per_; ++i){
		sitem = create_spike();
		sendBuf.push_back(sitem);
	}
    //gather how many events each neighbor is sending
	int size = sendBuf.size();
	sizeBuf.resize(num_procs_);
    MPI_Allgather(&size, 1, MPI_INT, &sizeBuf[0], 1, MPI_INT, MPI_COMM_WORLD);
}

void MpiSpikeGraph::allgatherv(){
    //get the displacements
    int displ[num_procs_];
    displ[0] = 0;
    for(int i=1; i < num_procs_; ++i){
		displ[i] = displ[i-1] + sizeBuf[i-1];
    }

    //next distribute items to every other process using allgatherv
	recvBuf.resize(events_per_ * num_procs_);
    MPI_Allgatherv(&(sendBuf[0]), sendBuf.size(), mpi_spikeItem_,
		   &recvBuf[0], &(sizeBuf[0]), displ, mpi_spikeItem_, MPI_COMM_WORLD);

	total_received_ += recvBuf.size();
}

bool MpiSpikeGraph::matches(const SpikeItem &sitem){
	for(int i = 0; i < inputPresyns_.size(); ++i){
		if(sitem.dst_ == inputPresyns_[i]){
				++total_relevent_;
				return true;
		}
	}
	return false;
}

void MpiSpikeGraph::reduce_stats(){
	//exchange total_received_
	int send = total_received_;
	if(rank_ == 0)
		MPI_Reduce(MPI_IN_PLACE, &send, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	else
		MPI_Reduce(&send, &send, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	total_received_ = send;

	//exchange total_relevent_
	send = total_relevent_;
	if(rank_ == 0)
		MPI_Reduce(MPI_IN_PLACE, &send, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	else
		MPI_Reduce(&send, &send, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	total_relevent_ = send;

	if(rank_ == 0){
		std::cout<<"Relevant events: "<<total_relevent_<<std::endl;
		std::cout<<"Received events: "<<total_received_<<std::endl;
	}
}

MPI_Datatype createMpiItemType(MPI_Datatype mpi_spikeItem){
    const int nblocks = 2;
    int blocklengths[2] = {1,1};
    MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};
    MPI_Aint offsets[2];

    offsets[0] = offsetof(SpikeItem, dst_);
    offsets[1] = offsetof(SpikeItem, t_);

    MPI_Type_create_struct(nblocks, blocklengths, offsets, types, &mpi_spikeItem);
    MPI_Type_commit(&mpi_spikeItem);
	return mpi_spikeItem;
}
