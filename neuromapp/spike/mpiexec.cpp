// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>

#include "spike/spike_exchange.h"
#include "utils/storage/neuromapp_data.h"
#include "spike/algos.hpp"

int main(int argc, char* argv[]) {
    assert(argc == 6);
    MPI::Init();
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    int eventsPer = atoi(argv[1]);
    int numOut= atoi(argv[2]);
    int simTime = atoi(argv[3]);
    int numIn = atoi(argv[4]);
    int isDistributed = atoi(argv[5]);

    assert(numIn <= (numOut * (size - 1)));

    spike::environment env(eventsPer, numOut, numIn, simTime, size, rank);

    if(isDistributed){
        spike::distributed d(size, rank);
        blocking(d, env);
        MPI_Type_free(&d.mpi_spike_item_);
    }
    else {
        spike::global_collective g;
        blocking(g, env);
        MPI_Type_free(&g.mpi_spike_item_);
    }
    MPI::Finalize();
    return 0;
}
