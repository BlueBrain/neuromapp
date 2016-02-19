// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <cassert>
#include <sys/time.h>

#include "utils/storage/neuromapp_data.h"
#include "spike/algos.hpp"
#include "spike/environment.h"
#include "utils/mpi/controler.h"
#include "coreneuron_1.0/queueing/pool.h"

#ifdef _OPENMP
    #include <omp.h>
#endif


int main(int argc, char* argv[]) {
    assert(argc == 6);
    int size = mapp::master.size();
    int rank = mapp::master.rank();

    //int numThreads = atoi(argv[1]);
    int eventsPer = atoi(argv[1]);
    int numOut= atoi(argv[2]);
    int simTime = atoi(argv[3]);
    int numIn = atoi(argv[4]);
    int isDistributed = atoi(argv[5]);

    struct timeval start, end;
    assert(numIn <= (numOut * (size - 1)));

//    spike::environment env(eventsPer, numOut, numIn, simTime, size, rank);
    //numcells, eventsper, percent ite, verbose, algebra, percent spike, numout, numin, size, rank
    queueing::pool<queueing::mutex> env(64, eventsPer, 90, false, true, 4, numOut, numIn, size, rank);

    gettimeofday(&start, NULL);
    run_sim(env, simTime, false);
    gettimeofday(&end, NULL);
    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec)) + ((end.tv_usec - start.tv_usec) / 1000);
        std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;
        std::cout<<"Process zero received: "<<env.received()<<" spikes"<<std::endl;
        std::cout<<"Process zero received: "<<env.relevent()<<" RELEVENT spikes"<<std::endl;
    return 0;
}
