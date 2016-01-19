// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>

#include "spike/mpispikegraph.h"
#include "spike/algos.hpp"

int main(int argc, char* argv[]) {
    assert(argc == 5);
    MPI::Init(argc, argv);

    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();
    int eventsPer = atoi(argv[1]);
    int numOut= atoi(argv[2]);
    int simTime = atoi(argv[3]);
    int numIn = atoi(argv[4]);

    assert(numIn < (size * numOut));

    DistributedSpikeGraph dsg(size, rank, numOut, numIn);
    dsg.setup();
    std::vector<SpikeItem> sendBuf;
    std::vector<SpikeItem> recvBuf;
    std::vector<int> sizeBuf;

    for(int i = 0; i < simTime; ++i){
	//generate messages (or not) every time step.
//	generate_spikes<DistributedSpikeGraph, SpikeItem, std::vector<SpikeItem> >
	(dsg, eventsPer, sendBuf);
	//exchange messages every 5dt
	if((i % 5) == 0){
//	    allgather< DistributedSpikeGraph>(dsg, sendBuf.size(), sizeBuf);
//	    allgatherv< DistributedSpikeGraph>(dsg,sendBuf,sizeBuf,recvBuf);
//	    filter< DistributedSpikeGraph, std::vector<SpikeItem> >(dsg, recvBuf.size(), recvBuf);
	}
    }

    dsg.freeMpiItemType();
    MPI::Finalize();
    return 0;
}
