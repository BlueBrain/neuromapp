// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>

#include "utils/storage/neuromapp_data.h"
#include "spike/algos.hpp"
#include "spike/environment.h"
#include "coreneuron_1.0/queueing/pool.h"

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

    assert(numIn <= (numOut * (size - 1)));

//    spike::environment env(eventsPer, numOut, numIn, simTime, size, rank);
    queueing::pool<queueing::mutex> env(64, eventsPer, 90, false, true, 4, numOut, numIn, size, rank);

    run_sim(env, simTime, false);

//    std::cout<<env.received()<<std::endl;
  //  std::cout<<env.all_matching()<<std::endl;

    MPI::Finalize();
    return 0;
}
