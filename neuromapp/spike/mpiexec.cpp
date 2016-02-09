// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>

#include "spike/mpispikegraph.h"
#include "utils/storage/neuromapp_data.h"
#include "spike/algos.hpp"

template<class T>
void run_sim(T& sg){
    sg.setup();

    //generate messages (or not) every time step.
    spike_exchange(sg);

    size_t result = 0;
    for(size_t i = 0; i < sg.recv_buf_.size(); ++i){
        if(sg.matches(sg.recv_buf_[i]))
            ++result;
    }
    sg.reduce_stats();
}

int main(int argc, char* argv[]) {
    assert(argc == 6);

    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();
    int eventsPer = atoi(argv[1]);
    int numOut= atoi(argv[2]);
    int simTime = atoi(argv[3]);
    int numIn = atoi(argv[4]);
    int isDistributed = atoi(argv[5]);

    assert(numIn <= (numOut * (size - 1)));

    if(isDistributed){
        DistributedSpikeGraph dsg(size, rank, numOut, numIn, eventsPer, simTime);
        dsg.generate_spikes();
        run_sim(dsg);
    }
    else {
        MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, simTime);
        sg.generate_spikes();
        run_sim(sg);
    }

    return 0;
}
