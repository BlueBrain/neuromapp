#include <iostream>
#include <stdlib.h>
#include <cstddef>
#include <mpi.h>
#include <cassert>

#include "spike/spike_exchange.h"

namespace spike{

distributed::distributed(int size, int rank){
    size_ = size;
    rank_ = rank;
    mpi_spike_item_ = create_mpi_spike_type(mpi_spike_item_);
    out_neighbors_.push_back((rank_ + 1)%size);
    if(rank == 0)
        in_neighbors_.push_back(size - 1);
    else
        in_neighbors_.push_back(rank - 1);

    MPI_Dist_graph_create_adjacent(MPI_COMM_WORLD, in_neighbors_.size(),
    &in_neighbors_[0], MPI_UNWEIGHTED, out_neighbors_.size(), &out_neighbors_[0],
    MPI_UNWEIGHTED, MPI_INFO_NULL, false, &neighborhood_);

}

void distributed::allgather(const int size, int_vec& nin){
    MPI_Neighbor_allgather(&size, 1, MPI_INT, &nin[0], 1, MPI_INT, neighborhood_);
}

void distributed::allgatherv(const spike_vec& spikeout, spike_vec& spikein,
const int_vec& nin, const int_vec& displ){
//next distribute items to every other process using allgatherv
    MPI_Neighbor_allgatherv(&spikeout[0], spikeout.size(), mpi_spike_item_,
    &spikein[0], &nin[0], &displ[0], mpi_spike_item_, neighborhood_);
}

MPI_Request distributed::Iallgather(const int size, int_vec& nin, MPI_Request request){
/*    MPI_Neighbor_Iallgather(&size, 1, MPI_INT, &nin[0], 1, MPI_INT, neighborhood_, &request);
    return request;*/
}

MPI_Request distributed::Iallgatherv(const spike_vec& spikeout, spike_vec& spikein,
const int_vec& nin, const int_vec& displ, MPI_Request request){
/*    MPI_Neighbor_Iallgatherv(&spikeout[0], spikeout.size(), mpi_spike_item_,
    &spikein[0], &nin[0], &displ[0], mpi_spike_item_, neighborhood_, &request);
    return request;*/
}

int distributed::get_status(MPI_Request request, int flag){
    MPI_Request_get_status(request, &flag, MPI_STATUS_IGNORE);
    return flag;
}

void distributed::wait(MPI_Request request){
    MPI_Wait(&request, MPI_STATUS_IGNORE);
}

/*
void distributed::setup(){
    MpiSpikeGraph::setup();
    //fill isInputNeighbor using inputPresyn info
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
            in_neighbors_.push_back(i);
    }

    if(rank_ == 0){
        std::cout<<"MPI Process "<<rank_<<" has "<<in_neighbors_.size()<<" in neighbors: ";
        for(int i = 0; i < in_neighbors_.size(); ++i){
            std::cout<<in_neighbors_[i]<<", ";
        }
        std::cout<<std::endl;
    }

    int inCnt = in_neighbors_.size();
    MPI_Allgather(&inCnt, 1, MPI_INT, &size_buf_[0], 1, MPI_INT, MPI_COMM_WORLD);
    set_displ();

    //distribute the inNeighbor info to every other process using allgatherv
    //use this to generate outNeighbor list
    std::vector<int> allInNeighbors;
    allInNeighbors.resize(num_procs_ * num_procs_);
    MPI_Allgatherv(&in_neighbors_[0], in_neighbors_.size(),
    MPI_INT, &allInNeighbors[0], &size_buf_[0], &displ_[0], MPI_INT, MPI_COMM_WORLD);

    for(int i = 0; i < num_procs_; ++i){
        for(int j = displ_[i]; j < (displ_[i] + size_buf_[i]); ++j){
            //if I am the inNeighbor for another node,
            //add that node to my out_neighbors_ list
            if(allInNeighbors[j] == rank_){
                out_neighbors_.push_back(i);
            }
        }
    }

    if(rank_ == 0){
        std::cout<<"MPI Process "<<rank_<<" has "<<out_neighbors_.size()<<" out neighbors: ";
        for(int i = 0; i < out_neighbors_.size(); ++i){
            std::cout<<out_neighbors_[i]<<", ";
        }
        std::cout<<std::endl;
    }

    MPI_Dist_graph_create_adjacent(MPI_COMM_WORLD, in_neighbors_.size(),
    &in_neighbors_[0],MPI_UNWEIGHTED, out_neighbors_.size(), &out_neighbors_[0],
    MPI_UNWEIGHTED, MPI_INFO_NULL, false, &neighborhood_);
}
*/
}
