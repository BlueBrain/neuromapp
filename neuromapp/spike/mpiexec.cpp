// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>

#include "mpispike.h"

int main(int argc, char* argv[]) {
    assert(argc == 6);
    MPI::Init(argc, argv);

    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();
    int meanEventsPerDT = atoi(argv[1]);
    int gidsPerProc = atoi(argv[2]);
    int simTime = atoi(argv[3]);
    int dt = atoi(argv[4]);
    int sdev = atoi(argv[5]);

    MpiSpike ms(size, rank, meanEventsPerDT, gidsPerProc);
    for(int i = 0; i < simTime; ++i){
	//generate messages (or not) every time step.
	ms.generateEvents();
	//exchange messages every 5dt
	if((i % 5) == 0){
	    ms.exchangeSizes();
	    ms.exchangeEvents();
	    ms.filterEvents();
	}
	//simulate dt wait time
	//wait(dt) +- (rand() % sdev);
    }

    ms.freeMpiItemType();
    MPI::Finalize();
    return 0;
}
