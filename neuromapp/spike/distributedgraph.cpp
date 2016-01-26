#include <iostream>
#include <stdlib.h>
#include <cstddef>
#include <mpi.h>
#include <cassert>
#include "spike/mpispikegraph.h"

void DistributedSpikeGraph::setup(){
    MpiSpikeGraph::setup();

    //fill inNeighbors using inputPresyn info
    std::vector<int> isInputNeighbor(num_procs_, 0);
    int neighborNum;
    for(int i = 0; i < inputPresyns_.size(); ++i){
		neighborNum = inputPresyns_[i] / num_out_;
		++isInputNeighbor[neighborNum];
    }

	//create edges for neighbor processes
	//no more than one edge can exist for each neighbor process
    for(int i = 0; i < num_procs_; ++i){
		if(isInputNeighbor[i])
	    	inNeighbors_.push_back(i);
    }

    if(rank_ == 0){
		std::cout<<"MPI Process "<<rank_<<" has "<<inNeighbors_.size()<<" in neighbors: ";
		for(int i = 0; i < inNeighbors_.size(); ++i){
		    std::cout<<inNeighbors_[i]<<", ";
		}
		std::cout<<std::endl;
    }

    int allInCnt[num_procs_];
    int inCnt = inNeighbors_.size();
    MPI_Allgather(&inCnt, 1, MPI_INT, allInCnt, 1, MPI_INT, MPI_COMM_WORLD);
    int displ[num_procs_];
    displ[0] = 0;
    for(int i = 1; i < num_procs_; ++i){
        displ[i] = displ[i-1] + allInCnt[i-1];
    }

    //distribute the inNeighbor info to every other process using allgatherv
    //use this to generate outNeighbor list
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

void DistributedSpikeGraph::allgather(){
	SpikeItem sitem;
    sendBuf.clear();
	for(int i = 0; i < events_per_; ++i){
		sitem = create_spike();
		sendBuf.push_back(sitem);
	}
    //gather how many events each neighbor is sending
	sizeBuf.resize(inNeighbors_.size());
	int size = events_per_;
    MPI_Neighbor_allgather(&size, 1, MPI_INT, &sizeBuf[0], 1,
	MPI_INT, neighborhood_);
}

void DistributedSpikeGraph::allgatherv(){
    //get the displacements
    int displ[inNeighbors_.size()];
    displ[0] = 0;
    if(inNeighbors_.size() > 0){
        for(int i=1; i < inNeighbors_.size(); ++i){
	    	displ[i] = displ[i-1] + sizeBuf[i-1];
		}
	}

    //next distribute items to every other process using allgatherv
	recvBuf.resize(events_per_ * inNeighbors_.size());
    MPI_Neighbor_allgatherv(&sendBuf[0], sendBuf.size(), mpi_spikeItem_,
		   &recvBuf[0], &sizeBuf[0], displ, mpi_spikeItem_, neighborhood_);

	total_received_ += recvBuf.size();
}
