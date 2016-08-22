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

#include <mpix.h>

#include "hdf5/H5SynapseLoader.h"


int main(int argc, char* argv[]) {
    assert(argc >= 3);

    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::string h5file(argv[1]);
    std::vector< std::string > datasets;
    for (int i=2; i<argc; i++)
        datasets.push_back(argv[i]);

    struct timeval start, end;

    //INITIALIZE
    uint64_t n_readSynapses=0;
    uint64_t n_SynapsesInDatasets=0;
    uint64_t fixed_num_syns=524288;

    H5SynapsesLoader loader(h5file, datasets,
            n_readSynapses,
            n_SynapsesInDatasets,
            fixed_num_syns);
    
    //INFO section
    std::cout << "rank=" << rank << " io-distance=" << MPIX_IO_distance() << std::endl;  
    gettimeofday(&start, NULL);

    std::vector<int> buffer;

    struct timeval it_start, it_end;
    while( !loader.eof() ) {
        gettimeofday(&it_start, NULL);
        loader.iterateOverSynapsesFromFiles(buffer);
        gettimeofday(&it_end, NULL);
        long long diff_ms = (1000 * (it_end.tv_sec - it_start.tv_sec))
                + ((it_end.tv_usec - it_start.tv_usec) / 1000);

        std::cout << "rank=" << rank << " time=" << diff_ms << "ms" << "\n";
    }

    gettimeofday(&end, NULL);

    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec))
        + ((end.tv_usec - start.tv_usec) / 1000);

    std::cout<<"rank="<<rank<<" run time="<<diff_ms<<"ms"<<std::endl;

    //pl.accumulate_stats();
    //accumulate_stats(s_interface);

    MPI_Finalize();
    return 0;
}
