/*
 * Neuromapp - distributed_driver.cpp, Copyright (c), 2015,
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
 * @file neuromapp/event_passing/drivers/distributed_driver.cpp
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
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"
#include "coreneuron_1.0/event_passing/spike/spike_interface.h"
#include "coreneuron_1.0/event_passing/spike/algos.hpp"
#include "coreneuron_1.0/event_passing/spike/distributed.hpp"
#include "utils/storage/neuromapp_data.h"

#ifdef _OPENMP
    #include <omp.h>
#endif

int main(int argc, char* argv[]) {

    assert(argc == 9);

    MPI_Init(NULL, NULL);
    MPI_Datatype mpi_spike = create_spike_type();
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int ngroups = atoi(argv[1]);
    int simtime = atoi(argv[2]);
    int out = atoi(argv[3]);
    int in = atoi(argv[4]);
    int netconsper = atoi(argv[5]);
    int nSpikes = atoi(argv[6]);
    int mindelay = atoi(argv[7]);
    bool algebra = atoi(argv[8]);

    struct timeval start, end;
    assert(in <= (out * (size - 1)));

    //create environment
    environment::event_generator generator(nSpikes, simtime, ngroups, rank, size, out);
    environment::presyn_maker presyns(out, in, netconsper);
    presyns(size, ngroups, rank);
    spike::spike_interface s_interface(size);

    //run simulation
    MPI_Comm neighborhood = create_dist_graph(presyns, out);
    queueing::pool pl(algebra, ngroups, mindelay, rank, s_interface);
    gettimeofday(&start, NULL);
    while(pl.get_time() <= simtime){
        pl.fixed_step(generator, presyns);
        distributed_spike(s_interface, mpi_spike, neighborhood);
        pl.filter(presyns);
    }
    gettimeofday(&end, NULL);

    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec))
        + ((end.tv_usec - start.tv_usec) / 1000);

    if(rank == 0){
        std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;
    }


    MPI_Comm_free(&neighborhood);
    MPI_Type_free(&mpi_spike);
    MPI_Finalize();
    return 0;
}
