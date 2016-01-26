// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>

#include "spike/mpispikegraph.h"
#include "utils/storage/neuromapp_data.h"
#include "spike/algos.hpp"

int main(int argc, char* argv[]) {
    assert(argc == 5);
    MPI::Init(argc, argv);
	MPI_Datatype mpi_spikeItem = createMpiItemType(mpi_spikeItem);

    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();
    int eventsPer = atoi(argv[1]);
    int numOut= atoi(argv[2]);
    int simTime = atoi(argv[3]);
    int numIn = atoi(argv[4]);

    assert(numIn <= (numOut * (size - 1)));

    MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);

//	if(isDistrubuted)
//    	sg = DistributedSpikeGraph(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);

    sg.setup();

	//generate messages (or not) every time step.
    allgather(sg);
    allgatherv(sg);

    size_t result = 0;
    for(size_t i = 0; i < sg.recvBuf.size(); ++i){
		if(sg.matches(sg.recvBuf[i]))
		    ++result;
    }

	sg.reduce_stats();

    MPI_Type_free(&mpi_spikeItem);
    MPI::Finalize();
    return 0;
}
