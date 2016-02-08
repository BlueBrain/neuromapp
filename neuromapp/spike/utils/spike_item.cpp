#include "spike/utils/spike_item.h"
#include <stdlib.h>
#include <cstddef>

MPI_Datatype create_mpi_spike_type(MPI_Datatype spike_item_dt){
    const int nblocks = 2;
    int blocklengths[2] = {1,1};
    MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};
    MPI_Aint offsets[2];

    offsets[0] = offsetof(spike_item, dst_);
    offsets[1] = offsetof(spike_item, t_);

    MPI_Type_create_struct(nblocks, blocklengths, offsets, types, &spike_item_dt);
    MPI_Type_commit(&spike_item_dt);
    return spike_item_dt;
}
