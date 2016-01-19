#include <iostream>
#include <stdlib.h>
#include <cstddef>
#include <mpi.h>
#include <cassert>
#include "spike/mpispikegraph.h"

DistributedSpikeGraph::~DistributedSpikeGraph(){
    inNeighbors_.clear();
    outNeighbors_.clear();
}

void DistributedSpikeGraph::setup(){
    MpiSpikeGraph::setup();

    //fill inNeighbors using inputPresyn info
    std::vector<int> isInputNeighbor(num_procs_, 0);
    int neighborNum;
    for(int i = 0; i < inputPresyns_.size(); ++i){
	neighborNum = inputPresyns_[i] / num_out_;
	++isInputNeighbor[neighborNum];
    }

    for(int i = 0; i < num_procs_; ++i){
	if(isInputNeighbor[i]){
	    inNeighbors_.push_back(i);
	}
    }

    if(rank_ == 0){
	std::cout<<"MPI Process "<<rank_<<" has "<<inNeighbors_.size()<<" in neighbors: ";
	for(int i = 0; i < inNeighbors_.size(); ++i){
	    std::cout<<inNeighbors_[i]<<", ";
	}
	std::cout<<std::endl;
    }

    //exchange inCnts
    int allInCnt[num_procs_];
    int inCnt = inNeighbors_.size();
    MPI_Allgather(&inCnt, 1, MPI_INT, allInCnt, 1, MPI_INT, MPI_COMM_WORLD);
    //Get the displacements
    int displ[num_procs_];
    displ[0] = 0;
    for(int i = 1; i < num_procs_; ++i){
        displ[i] = displ[i-1] + allInCnt[i-1];
    }

    //Then distribute the inputPresyn info to every other process using allgatherv
    int allInNeighbors[num_procs_ * num_procs_];
    MPI_Allgatherv(&inNeighbors_[0], inNeighbors_.size(), MPI_INT, &(allInNeighbors),
  		   allInCnt, displ, MPI_INT, MPI_COMM_WORLD);

    for(int i = 0; i < num_procs_; ++i){
        for(int j = displ[i]; j < (displ[i] + allInCnt[i]); ++j){
            //if I am the inNeighbor for another node, add that node to my outNeighbors_ list
            if(allInNeighbors[j] == rank_){
		outNeighbors_.push_back(i);
	    }
	}
    }

    if(rank_ == 0){
	std::cout<<"MPI Process "<<rank_<<" has "<<outNeighbors_.size()<<" out neighbors: ";
	for(int i = 0; i < outNeighbors_.size(); ++i){
	    std::cout<<outNeighbors_[i]<<", ";
	}
	std::cout<<std::endl;
    }

    MPI_Dist_graph_create_adjacent(MPI_COMM_WORLD, inNeighbors_.size(), &inNeighbors_[0],
				   MPI_UNWEIGHTED, outNeighbors_.size(), &outNeighbors_[0],
				   MPI_UNWEIGHTED, MPI_INFO_NULL, false, &neighborhood_);
}

void DistributedSpikeGraph::allgather(int size, std::vector<int> &sizeBuf){
    //gather how many events each neighbor is sending
    MPI_Neighbor_allgather(&size, 1, MPI_LONG_LONG_INT, &sizeBuf[0], 1,
MPI_LONG_LONG_INT, neighborhood_);
}

void DistributedSpikeGraph::allgatherv(std::vector<SpikeItem> &send,
std::vector<int> &sizeBuf, std::vector<SpikeItem> &recv){
    //get the displacements
    int totalCnt;
    int displ[inNeighbors_.size()];
    displ[0] = 0;
    if(inNeighbors_.size() > 0){
        totalCnt = sizeBuf[0];
        for(int i=1; i < inNeighbors_.size(); ++i){
	    totalCnt += sizeBuf[i];
	    displ[i] = displ[i-1] + sizeBuf[i-1];
	}
    }
    else{
	totalCnt = 0;
    }

    //next distribute items to every other process using allgatherv
    MPI_Neighbor_allgatherv(&send[0], send.size(), mpi_spikeItem,
		   &recv[0], &sizeBuf[0], displ, mpi_spikeItem, neighborhood_);

    send.clear();
    sizeBuf.clear();
}


