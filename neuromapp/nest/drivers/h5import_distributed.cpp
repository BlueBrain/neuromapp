/*
 * Neuromapp - distributed_driver.cpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/event_passing/drivers/distributed_driver.cpp
 * runs the spike exchange simulation
 */
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <cassert>
#include <sys/time.h>

#include "utils/storage/neuromapp_data.h"

#ifdef _OPENMP
    #include <omp.h>
#endif

#include "nest/h5import/fakenestkernel/nest_kernel.h"
#include "nest/h5import/H5Synapses.h"


using namespace h5import;


int main(int argc, char* argv[]) {
    assert(argc == 4);

    MPI_Init(NULL, NULL);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int nthreads = atoi(argv[1]);
    const int ncells = atoi(argv[2]);
    std::string syn_file(argv[3]);

#ifdef _OPENMP
	omp_set_num_threads(nthreads);
#else
	nthreads = 1;
	std::cout << "WARNING: OpenMP not available! num_threads is set to 1.";
#endif

    //load kernel environment
    kernel_env kenv(ncells, nthreads, rank, size);

    struct timeval start, end;
	
     //run simulation
    gettimeofday(&start, NULL);

    H5Synapses h5synapses;
    h5synapses.set_filename(syn_file);
    std::vector<std::string> props;
    props.push_back("delay");
    props.push_back("weight");
	props.push_back("U0");
	props.push_back("TauRec");
	props.push_back("TauFac");
    h5synapses.set_parameters(props);

    GIDCollection gids;
    h5synapses.set_mapping(gids);
    h5synapses.import();

    gettimeofday(&end, NULL);

    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec))
        + ((end.tv_usec - start.tv_usec) / 1000);

    std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;

    long num_connections=0;
    for (int thrd=0; thrd<nthreads; thrd++) {
        num_connections += kernel().connection_manager.num_connections[thrd];
    }
    std::cout<<"stats: num_connections="<<num_connections<<std::endl;

    MPI_Finalize();
    return 0;
}
