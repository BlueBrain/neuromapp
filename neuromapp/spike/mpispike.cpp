#include <iostream>
#include <stdlib.h>
#include <mpi.h>

#include "spike/mpispike.h"

MpiSpike::MpiSpike(int n, int rank, int meanEventsPerDT, int gidsPerProc){
    int minDelay = 5;
    num_procs_ = n;
    sizeBuf_ = new int[n];
    events_per_ = meanEventsPerDT;
    gids_per_ = gidsPerProc;
    totalCnt_ = 0;
    totalSent_ = 0;
    recvd_ = 0;
    recvBuf_.reserve(num_procs_*events_per_*minDelay);
    rank_ = rank;
    srand(time(NULL)+rank_);

    createMpiItemType();
}

MpiSpike::~MpiSpike(){
    delete [] sizeBuf_;
    recvBuf_.clear();
    sendBuf_.clear();
    if(rank_ == 0){
	std::cout<<"Total events sent: "<<totalSent_<<std::endl;
    }
    std::cout<<"MPI process "<<rank_<<" received: "<<recvd_<<" events"<<std::endl;
}

void MpiSpike::createMpiItemType(){
    const int nblocks = 2;
    int blocklengths[2] = {1,1};
    MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};
    MPI_Aint offsets[2];

    offsets[0] = offsetof(SpikeItem, dst_);
    offsets[1] = offsetof(SpikeItem, t_);

    MPI_Type_create_struct(nblocks, blocklengths, offsets, types, &mpi_spikeItem);
    MPI_Type_commit(&mpi_spikeItem);
}

void MpiSpike::freeMpiItemType(){
    MPI_Type_free(&mpi_spikeItem);
}

void MpiSpike::generateEvents(){
    //clear send buffer
    sendBuf_.clear();

    //Select a random number of items to generate
    if(num_procs_ > 1){
	int myCnt = (rand() % (2 * events_per_));
	int lBound = rank_ * gids_per_;
	int uBound = lBound + gids_per_;
	for(int i = 0; i < myCnt; ++i){
	    SpikeItem sitem;
	    int dst = lBound;
	    //All gid's in range lBound to uBound belong to me
	    //Cant send to my own gid's
	    while(dst >= lBound && dst < uBound){
		dst = rand() % (num_procs_ * gids_per_);
	    }
	    sitem.dst_ = dst;
	    sitem.t_ = (rand() / (double)RAND_MAX);
	    sendBuf_.push_back(sitem);
    	}
    }
}

void MpiSpike::exchangeSizes(){
    int size = sendBuf_.size();
    MPI_Allgather(&size, 1, MPI_INT, sizeBuf_, 1, MPI_INT, MPI_COMM_WORLD);
}

void MpiSpike::exchangeEvents(){
    //get the displacements
    int displ[num_procs_];
    totalCnt_ = sizeBuf_[0];
    displ[0] = 0;
    for(int i=1; i < num_procs_; ++i){
	totalCnt_ += sizeBuf_[i];
	displ[i] = displ[i-1] + sizeBuf_[i-1];
    }
    totalSent_ += totalCnt_;

    //clear recvBuf_
    recvBuf_.clear();

    //next distribute items to every other process using allgatherv
    MPI_Allgatherv(&sendBuf_[0], sendBuf_.size(), mpi_spikeItem,
		   &recvBuf_[0], sizeBuf_, displ, mpi_spikeItem, MPI_COMM_WORLD);
}

void MpiSpike::filterEvents(){
    //own gid's exist within the range between lbound (lower) - ubound (upper)
    int lBound = rank_ * gids_per_;
    int uBound = lBound + gids_per_;
    int dst;
    for(int i = 0; i < totalCnt_; ++i){
	//if own gid
	if(recvBuf_[i].dst_ >= lBound && recvBuf_[i].dst_ < uBound){
	    ++recvd_;
	}
    }
}
