#include <iostream>
#include <stdlib.h>
#include <cstddef>
#include <mpi.h>
#include <cassert>
#include "spike/mpispikegraph.h"

void FullyConnectedGraph::generateEvents(){
    //clear send buffer
    sendBuf_.clear();

    //Select a random number of items to generate
    int myCnt = (rand() % (2 * events_per_));
    for(int i = 0; i < myCnt; ++i){
        SpikeItem sitem;
        //select the dstProcess
        int index = rand() % outputPresyns_.size();
        int dstProc = outputPresyns_[index];
        //Specify the GID
        int dstGid = rand() % gids_per_;
        //All gid's in range lBound to uBound belong to me
        //Cant send to my own gid's
        sitem.dst_ = (dstProc * gids_per_) + dstGid;
        sitem.t_ = (rand() / (double)RAND_MAX);
        sendBuf_.push_back(sitem);
    }
}

void FullyConnectedGraph::exchangeSizes(){
    int size = sendBuf_.size();
    sizeBuf_.clear();
    MPI_Allgather(&size, 1, MPI_INT, &sizeBuf_[0], 1, MPI_INT, MPI_COMM_WORLD);
}

void FullyConnectedGraph::exchangeEvents(){
    //get the displacements
    int totalCnt;
    int displ[inputPresyns_.size()];
    displ[0] = 0;
    if(inputPresyns_.size() > 0){
        totalCnt = sizeBuf_[0];
        for(int i=1; i < inputPresyns_.size(); ++i){
	    totalCnt += sizeBuf_[i];
	    displ[i] = displ[i-1] + sizeBuf_[i-1];
	}
    }
    else{
	totalCnt = 0;
    }

    //clear recvBuf_
    recvBuf_.clear();
    total_sent_ += sendBuf_.size();
    //next distribute items to every other process using allgatherv
    MPI_Neighbor_allgatherv(&sendBuf_[0], sendBuf_.size(), mpi_spikeItem,
		   &recvBuf_[0], &sizeBuf_[0], displ, mpi_spikeItem, MPI_COMM_WORLD);
}
