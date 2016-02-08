#include <mpi.h>

#ifndef spike_item_h
#define spike_item_h

struct spike_item{
	int dst_;
	double t_;
};

/** \fn create_mpi_spike_type(MPI_Datatype)
    \brief creates the type mpi_spikeItem in order to send SpikeItems using MPI
 */
MPI_Datatype create_mpi_spike_type(MPI_Datatype spike_item_dt);

#endif
