/*
 * Neuromapp - event.cpp, Copyright (c), 2015,
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
 * @file neuromapp/event_passing/drivers/event.cpp
 * runs the spike exchange simulation
 */
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <cassert>
#include <sys/time.h>

#include "coreneuron_1.0/event_passing/queueing/queue.h"
#include "coreneuron_1.0/event_passing/queueing/pool.h"
#include "coreneuron_1.0/event_passing/queueing/thread.h"
#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/event_generators.hpp"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"
#include "coreneuron_1.0/event_passing/spike/spike_interface.h"
#include "coreneuron_1.0/event_passing/spike/algos.hpp"
#include "coreneuron_1.0/event_passing/drivers/drivers.h"
#include "utils/storage/neuromapp_data.h"

// Get OMP header if available
#include "utils/omp/compatibility.h"

int main(int argc, char* argv[]) {

    assert(argc == 8);

    MPI_Init(NULL, NULL);
    MPI_Datatype mpi_spike = create_spike_type();
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int ngroups = atoi(argv[1]);
    int simtime = atoi(argv[2]);
    int ncells = atoi(argv[3]);
    int fanin = atoi(argv[4]);
    int nSpikes = atoi(argv[5]);
    int mindelay = atoi(argv[6]);
    bool algebra = atoi(argv[7]);

    struct timeval start, end;

    //create environment
    environment::event_generator generator(ngroups);

    double mean = static_cast<double>(simtime) / static_cast<double>(nSpikes);
    double lambda = 1.0 / static_cast<double>(mean * size);

    environment::continousdistribution neuro_dist(size, rank, ncells);

    environment::generate_events_kai(generator.begin(),
                             simtime, ngroups, rank, size, lambda, &neuro_dist);

    environment::presyn_maker presyns(fanin);
    presyns(rank, &neuro_dist);
    spike::spike_interface s_interface(size);
    //run simulation
    queueing::pool pl(algebra, ngroups, mindelay, rank, s_interface);
    gettimeofday(&start, NULL);
    int cntr = 0;
    while(pl.get_time() <= simtime){
        pl.fixed_step(generator, presyns);
        blocking_spike(s_interface, mpi_spike);
        pl.filter(presyns);
    }
    gettimeofday(&end, NULL);

    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec))
        + ((end.tv_usec - start.tv_usec) / 1000);

    if(rank == 0)
        std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;

    pl.accumulate_stats();
    accumulate_stats(s_interface);

    MPI_Type_free(&mpi_spike);
    MPI_Finalize();
    return 0;
}
