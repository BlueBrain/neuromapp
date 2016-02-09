// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>

#include "spike/mpispikegraph.h"
#include "utils/storage/neuromapp_data.h"
#include "spike/algos.hpp"

int main(int argc, char* argv[]) {
    assert(argc == 6);

    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();
   

    mpi_spike_graph sg;

    if(isDistributed){
        sg = distributed_graph(int size, int rank);
    }
    else {
        sg = blocking_global_graph();
    }
    sg.setup();

    spike::spike_vec spikein;
    spike::spike_vec spikeout;
    spike::int_vec nin;
    spike::int_vec displ;

    set_displ(size, nin, displ);

    spike_exchange(sg);

    return 0;
}
