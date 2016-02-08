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
    for(int i = 0; i < input_presyns_.size(); ++i){
        neighborNum = input_presyns_[i] / num_out_;
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

    int inCnt = inNeighbors_.size();
    MPI_Allgather(&inCnt, 1, MPI_INT, &size_buf_[0], 1, MPI_INT, MPI_COMM_WORLD);
    set_displ();

    //distribute the inNeighbor info to every other process using allgatherv
    //use this to generate outNeighbor list
    std::vector<int> allInNeighbors;
    allInNeighbors.resize(num_procs_ * num_procs_);
    MPI_Allgatherv(&inNeighbors_[0], inNeighbors_.size(),
    MPI_INT, &(allInNeighbors), &size_buf_[0], &displ_[0], MPI_INT, MPI_COMM_WORLD);

    for(int i = 0; i < num_procs_; ++i){
        for(int j = displ_[i]; j < (displ_[i] + size_buf_[i]); ++j){
            //if I am the inNeighbor for another node,
            //add that node to my outNeighbors_ list
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

    MPI_Dist_graph_create_adjacent(MPI_COMM_WORLD, inNeighbors_.size(),
    &inNeighbors_[0],MPI_UNWEIGHTED, outNeighbors_.size(), &outNeighbors_[0],
    MPI_UNWEIGHTED, MPI_INFO_NULL, false, &neighborhood_);
}

void DistributedSpikeGraph::allgather(){
    int size = send_buf_.size();
    MPI_Neighbor_allgather(&size, 1, MPI_INT, &size_buf_[0], 1,
    MPI_INT, neighborhood_);
}

void DistributedSpikeGraph::allgatherv(){
//next distribute items to every other process using allgatherv
    MPI_Neighbor_allgatherv(&send_buf_[0], send_buf_.size(), mpi_spike_item_,
    &recv_buf_[0], &size_buf_[0], &displ_[0], mpi_spike_item_, neighborhood_);
}
