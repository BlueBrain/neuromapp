/*
 * Neuromapp - mpiexec.cpp, Copyright (c), 2015,
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
 * @file neuromapp/spike/mpiexec.cpp
 * runs the spike exchange simulation
 */
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <cassert>
#include <sys/time.h>

#include "spike/algos.hpp"
#include "spike/environment.h"
#include "utils/storage/neuromapp_data.h"
#include "utils/mpi/controler.h"
#include "coreneuron_1.0/queueing/pool.h"

#ifdef _OPENMP
    #include <omp.h>
#endif


/**
 * \brief instantiate an environment variable(either spike::environment or queueing::pool)
 * and performs run_simulation() function on it.
 */
int main(int argc, char* argv[]) {
    assert(argc == 7);
    int size = mapp::master.size();
    int rank = mapp::master.rank();

    int eventsPer = atoi(argv[1]);
    int simTime = atoi(argv[2]);
    int numOut = atoi(argv[3]);
    int numIn = atoi(argv[4]);
    int netconsPer = atoi(argv[5]);
    int isNonBlocking = atoi(argv[6]);

    struct timeval start, end;
    assert(numIn <= (numOut * (size - 1)));
    //numcells, eventsper, percent ite, verbose, algebra, percent spike, numout, numin, size, rank
    queueing::pool env(64, eventsPer, 90, false, true, 4, numOut, numIn, netconsPer, size, rank);

    gettimeofday(&start, NULL);
    run_sim(env, simTime, isNonBlocking);
    gettimeofday(&end, NULL);

    env.accumulate_stats();
    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec)) + ((end.tv_usec - start.tv_usec) / 1000);
        std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;
        std::cout<<"Process zero received: "<<env.received()<<" spikes"<<std::endl;
        std::cout<<"Process zero received: "<<env.relevent()<<" RELEVENT spikes"<<std::endl;
    mapp::master.finalize();
    return 0;
}
