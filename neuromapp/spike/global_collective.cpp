#include <iostream>
#include <stdlib.h>
#include <cstddef>
#include <mpi.h>
#include <cassert>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>

#include "spike/spike_exchange.h"

namespace spike{

global_collective::global_collective(){
    mpi_spike_item_ = create_mpi_spike_type(mpi_spike_item_);
}

void global_collective::allgather(const int size, int_vec& nin){
    MPI_Allgather(&size, 1, MPI_INT, &nin[0], 1, MPI_INT, MPI_COMM_WORLD);
}

void global_collective::allgatherv(const spike_vec& spikeout,
spike_vec& spikein, const int_vec& nin, const int_vec& displ){
    MPI_Allgatherv(&spikeout[0], spikeout.size(), mpi_spike_item_,
    &spikein[0], &nin[0], &displ[0], mpi_spike_item_, MPI_COMM_WORLD);
}

MPI_Request global_collective::Iallgather(const int size, int_vec& nin, MPI_Request request){
    MPI_Iallgather(&size, 1, MPI_INT, &nin[0], 1, MPI_INT, MPI_COMM_WORLD, &request);
    return request;
}

MPI_Request global_collective::Iallgatherv(const spike_vec& spikeout,
spike_vec& spikein, const int_vec& nin, const int_vec& displ, MPI_Request request){
    MPI_Iallgatherv(&spikeout[0], spikeout.size(), mpi_spike_item_,
    &spikein[0], &nin[0], &displ[0], mpi_spike_item_, MPI_COMM_WORLD, &request);
    return request;
}

int global_collective::get_status(MPI_Request request, int flag){
    MPI_Request_get_status(request, &flag, MPI_STATUS_IGNORE);
    return flag;
}


void global_collective::wait(MPI_Request request){
    MPI_Wait(&request, MPI_STATUS_IGNORE);
}

}
/*

void global_collective::reduce_stats(){
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

bool global_collective::matches(const spike_item &sitem){
    for(int i = 0; i < input_presyns_.size(); ++i){
        if(sitem.dst_ == input_presyns_[i]){
            ++total_relevent_;
            return true;
        }
    }
    return false;
}
*/
