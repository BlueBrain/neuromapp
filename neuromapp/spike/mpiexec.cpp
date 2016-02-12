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
#include "coreneuron_1.0/queueing/pool.h"

#ifdef _OPENMP
    #include <omp.h>
#endif


int main(int argc, char* argv[]) {
    assert(argc == 6);
    MPI::Init();
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    //int numThreads = atoi(argv[1]);
    int eventsPer = atoi(argv[1]);
    int numOut= atoi(argv[2]);
    int simTime = atoi(argv[3]);
    int numIn = atoi(argv[4]);
    int isDistributed = atoi(argv[5]);

#ifdef _OPENMP
    omp_set_num_threads(8);
#endif

    struct timeval start, end;
    assert(numIn <= (numOut * (size - 1)));

//    spike::environment env(eventsPer, numOut, numIn, simTime, size, rank);
    queueing::pool<queueing::mutex> env(64, eventsPer, 90, false, true, 4, numOut, numIn, size, rank);

    gettimeofday(&start, NULL);
    run_sim(env, simTime, false);
    gettimeofday(&end, NULL);
    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec)) + ((end.tv_usec - start.tv_usec) / 1000);
    if(rank == 0)
        std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;
//    std::cout<<env.received()<<std::endl;
  //  std::cout<<env.all_matching()<<std::endl;

    MPI::Finalize();
    return 0;
}
