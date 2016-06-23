/*
 * Neuromapp - algos.hpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/event_passing/spike/algos.hpp
 * contains algorithm definitions for spike exchange
 */

#ifndef MAPP_ALGOS_H
#define MAPP_ALGOS_H

#include <assert.h>
#include <cstddef>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <mpi.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

#include "coreneuron_1.0/event_passing/queueing/queue.h"
#include "coreneuron_1.0/event_passing/spike/spike_interface.h"

//define events as spike_item
typedef queueing::event spike_item;

/**
 * \fn create_spike_type()
 * \brief creates an MPI_Datatype required for MPI
 *  communication of the spike_item struct
 * \return the new MPI_Datatype, spike
 */
inline MPI_Datatype create_spike_type(){
    MPI_Datatype spike;
    const int nblocks = 2;
    int blocklengths[2] = {1,1};
    MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};
    MPI_Aint offsets[2];

    offsets[0] = offsetof(spike_item, data_);
    offsets[1] = offsetof(spike_item, t_);

    MPI_Type_create_struct(nblocks, blocklengths, offsets, types, &spike);
    MPI_Type_commit(&spike);
    return spike;
}

/**
 * \fn barrier()
 * \brief performs an MPI_Barrier
 */
inline void barrier(){
    MPI_Barrier(MPI_COMM_WORLD);
}

//BLOCKING
/**
 * \fn allgather(data& d)
 * \brief performs the blocking collective, MPI_Allgather
 * \param d the data environment on which this algo is called
 */
template<typename data>
void allgather(data& d){
    int send_size = d.spikeout_.size();
    MPI_Allgather(&send_size, 1, MPI_INT, &(d.nin_[0]), 1, MPI_INT, MPI_COMM_WORLD);
}

template<>
void allgather<spike::spike_interface_stats_collector>(spike::spike_interface_stats_collector& d){
    int send_size = d.spikeout_.size();
    double t0,t1;
    t0 = MPI_Wtime();
    MPI_Allgather(&send_size, 1, MPI_INT, &(d.nin_[0]), 1, MPI_INT, MPI_COMM_WORLD);
    t1 = MPI_Wtime();
    d.allgather_times_.push_back(t1-t0);
}

template<>
void allgather<spike::spike_interface_stats_collector_large_mpi>(spike::spike_interface_stats_collector_large_mpi& d){
    int send_size = d.spikeout_.size();
    double t0,t1;
    t0 = MPI_Wtime();
    MPI_Allgather(&send_size, 1, MPI_INT, &(d.nin_[0]), 1, MPI_INT, MPI_COMM_WORLD);
    t1 = MPI_Wtime();
    d.allgather_times_.push_back(t1-t0);
}


/**
 * \fn allgatherv(data& d, MPI_Datatype spike)
 * \brief performs the blocking collective, MPI_Allgatherv
 * \param d the data environment on which this algo is called
 * \param spike the MPI_Datatype being communicated
 */
template<typename data>
void allgatherv(data& d, MPI_Datatype spike){
    MPI_Allgatherv(&(d.spikeout_[0]), d.spikeout_.size(), spike,
        &(d.spikein_[0]), &(d.nin_[0]), &(d.displ_[0]), spike, MPI_COMM_WORLD);
}

template<>
void allgatherv<spike::spike_interface_stats_collector>(spike::spike_interface_stats_collector& d, MPI_Datatype spike){
    double t0,t1;
    t0 = MPI_Wtime();
    MPI_Allgatherv(&(d.spikeout_[0]), d.spikeout_.size(), spike,
        &(d.spikein_[0]), &(d.nin_[0]), &(d.displ_[0]), spike, MPI_COMM_WORLD);
    t1 = MPI_Wtime();
    d.allgather_v_times_.push_back(std::make_pair(d.spikeout_.size(), t1-t0));
}

template<>
void allgatherv<spike::spike_interface_stats_collector_large_mpi>(spike::spike_interface_stats_collector_large_mpi& d, MPI_Datatype spike){
    double t0,t1;
    t0 = MPI_Wtime();
    MPI_Allgatherv(&(d.spikeout_[0]), d.spikeout_.size(), spike,
        &(d.spikein_[0]), &(d.nin_[0]), &(d.displ_[0]), spike, MPI_COMM_WORLD);
    t1 = MPI_Wtime();
    d.allgather_v_times_.push_back(t1-t0);
    d.allgather_v_sizes_.push_back(d.spikeout_.size());
}

/**
 * \fn set_displ(data& d)
 * \brief sets displacements needed for the algatherv
 * \param d the data environment on which this algo is called
 */
template<typename data>
void set_displ(data& d){
    d.displ_[0] = 0;
    int total = d.nin_[0];
    for(int i=1; i < d.nin_.size(); ++i){
        d.displ_[i] = total;
        total += d.nin_[i];
    }
    d.spikein_.resize(total);
}

template<typename data>
void accumulate_stats(data& d){
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Spike stats
    if (rank == 0){
        MPI_Reduce(MPI_IN_PLACE, &(d.spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.ite_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.local_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.post_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.received_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Reduce(&(d.spike_stats_), &(d.spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.ite_stats_), &(d.ite_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.local_stats_), &(d.local_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.post_spike_stats_), &(d.post_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.received_spike_stats_), &(d.received_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    if(rank == 0){
        std::cout<<"Total Spikes: "<<d.spike_stats_<<std::endl;
        std::cout<<"Total Inter-thread: "<<d.ite_stats_<<std::endl;
        std::cout<<"Total Local: "<<d.local_stats_<<std::endl;
        std::cout<<"Total Post-spike Events: "<<d.post_spike_stats_<<std::endl;
        std::cout<<"Total Received spikes: "<<d.received_spike_stats_<<std::endl;
    }
}

template<>
void accumulate_stats<spike::spike_interface_stats_collector>(spike::spike_interface_stats_collector& d){
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Spike stats
    if (rank == 0){
        MPI_Reduce(MPI_IN_PLACE, &(d.spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.ite_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.local_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.post_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.received_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Reduce(&(d.spike_stats_), &(d.spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.ite_stats_), &(d.ite_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.local_stats_), &(d.local_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.post_spike_stats_), &(d.post_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.received_spike_stats_), &(d.received_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    if(rank == 0){
        std::cout<<"Total Spikes: "<<d.spike_stats_<<std::endl;
        std::cout<<"Total Inter-thread: "<<d.ite_stats_<<std::endl;
        std::cout<<"Total Local: "<<d.local_stats_<<std::endl;
        std::cout<<"Total Post-spike Events: "<<d.post_spike_stats_<<std::endl;
        std::cout<<"Total Received spikes: "<<d.received_spike_stats_<<std::endl;
    }

    //std::cout <<"Printing Allgather times"<<std::endl;
    std::copy( d.allgather_times_.begin(), d.allgather_times_.end(), std::ostream_iterator<double>( d.outfile_allgather, "\n"));
    std::vector<std::pair<int,double> >::iterator it;
    for ( it = d.allgather_v_times_.begin(); it != d.allgather_v_times_.end(); it++ ) {
	d.outfile_allgather_v << it->first << "    " << it->second << std::endl;
    }
}

template<>
void accumulate_stats<spike::spike_interface_stats_collector_large_mpi>(spike::spike_interface_stats_collector_large_mpi& d){
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Spike stats
    if (rank == 0){
        MPI_Reduce(MPI_IN_PLACE, &(d.spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.ite_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.local_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.post_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(MPI_IN_PLACE, &(d.received_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Reduce(&(d.spike_stats_), &(d.spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.ite_stats_), &(d.ite_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.local_stats_), &(d.local_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.post_spike_stats_), &(d.post_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&(d.received_spike_stats_), &(d.received_spike_stats_), 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    if(rank == 0){
        std::cout<<"Total Spikes: "<<d.spike_stats_<<std::endl;
        std::cout<<"Total Inter-thread: "<<d.ite_stats_<<std::endl;
        std::cout<<"Total Local: "<<d.local_stats_<<std::endl;
        std::cout<<"Total Post-spike Events: "<<d.post_spike_stats_<<std::endl;
        std::cout<<"Total Received spikes: "<<d.received_spike_stats_<<std::endl;
    }

    if (rank == 0){

	std::cout << "Collecting statistical information from all the ranks" << std::endl;

	std::vector<int> sum_allgather_v_sizes(d.allgather_v_sizes_);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(MPI_IN_PLACE, &(*(d.allgather_times_.begin())), d.allgather_times_.size(), MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(MPI_IN_PLACE, &(*(d.allgather_v_times_.begin())), d.allgather_v_times_.size(), MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(MPI_IN_PLACE, &(*(sum_allgather_v_sizes.begin())), d.allgather_v_sizes_.size(), MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(MPI_IN_PLACE, &(*(d.allgather_v_sizes_.begin())), d.allgather_v_sizes_.size(), MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);

	std::cout << "Writing out data files" << std::endl;

	std::fstream outfile_allgather_t_max;
	std::fstream outfile_allgather_v_t_max;
	std::fstream outfile_allgather_v_s_max;
	std::fstream outfile_allgather_v_s_sum;

	std::stringstream outfile_name;

	outfile_name << d.outfile_prefix_.str() << "/allgather_times_c" << d.ncells_ << "_s" << d.nSpikes_ << "_d" << d.mindelay_ << "_t" << d.simtime_ << "_p" << d.nprocs_ << ".dat";
	outfile_allgather_t_max.open(outfile_name.str().c_str(), std::fstream::out);
	std::copy( d.allgather_times_.begin(), d.allgather_times_.end(), std::ostream_iterator<double>( outfile_allgather_t_max, "\n"));
	outfile_allgather_t_max.close();

	outfile_name.str( std::string() );
	outfile_name.clear();
	outfile_name << d.outfile_prefix_.str() << "/allgather_v_times_c" << d.ncells_ << "_s" << d.nSpikes_ << "_d" << d.mindelay_ << "_t" << d.simtime_ << "_p" << d.nprocs_ << ".dat";
	outfile_allgather_v_t_max.open(outfile_name.str().c_str(), std::fstream::out);
	std::copy( d.allgather_v_times_.begin(), d.allgather_v_times_.end(), std::ostream_iterator<double>( outfile_allgather_v_t_max, "\n"));
	outfile_allgather_v_t_max.close();

	outfile_name.str( std::string() );
	outfile_name.clear();
	outfile_name << d.outfile_prefix_.str() << "/allgather_v_sizes_max_c" << d.ncells_ << "_s" << d.nSpikes_ << "_d" << d.mindelay_ << "_t" << d.simtime_ << "_p" << d.nprocs_ << ".dat";
	outfile_allgather_v_s_max.open(outfile_name.str().c_str(), std::fstream::out);
	std::copy( d.allgather_v_sizes_.begin(), d.allgather_v_sizes_.end(), std::ostream_iterator<double>( outfile_allgather_v_s_max, "\n"));
	outfile_allgather_v_s_max.close();

	outfile_name.str( std::string() );
	outfile_name.clear();
	outfile_name << d.outfile_prefix_.str() << "/allgather_v_sizes_c" << d.ncells_ << "_s" << d.nSpikes_ << "_d" << d.mindelay_ << "_t" << d.simtime_ << "_p" << d.nprocs_ << ".dat";
	outfile_allgather_v_s_sum.open(outfile_name.str().c_str(), std::fstream::out);
	std::copy( sum_allgather_v_sizes.begin(), sum_allgather_v_sizes.end(), std::ostream_iterator<double>( outfile_allgather_v_s_sum, "\n"));
	outfile_allgather_v_s_sum.close();


    } else {
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&(*(d.allgather_times_.begin())), &(*(d.allgather_times_.begin())), d.allgather_times_.size(), MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&(*(d.allgather_v_times_.begin())), &(*(d.allgather_v_times_.begin())), d.allgather_v_times_.size(), MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&(*(d.allgather_v_sizes_.begin())), &(*(d.allgather_v_sizes_.begin())), d.allgather_v_sizes_.size(), MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&(*(d.allgather_v_sizes_.begin())), &(*(d.allgather_v_sizes_.begin())), d.allgather_v_sizes_.size(), MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
    }



}



//SIMULATIONS
/**
 * \fn blocking_spike(data& d, MPI_Datatype spike)
 * \brief performs a blocking spike exchange
 * \param d the data environment on which this algo is called
 */
template<typename data>
void blocking_spike(data& d, MPI_Datatype spike){
    //gather how many spikes each process is sending
    allgather(d);
    //set the displacements
    set_displ(d);
    //next distribute items to every other process using allgatherv
    allgatherv(d, spike);
}

#endif
