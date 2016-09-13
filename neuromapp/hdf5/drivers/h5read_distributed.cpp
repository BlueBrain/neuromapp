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

#include <boost/program_options.hpp>
#include "utils/storage/neuromapp_data.h"
#include "iobench/utils/stats.h"

#ifdef _OPENMP
    #include <omp.h>
#endif

#include "hdf5/h5reader.h"

int main(int argc, char* argv[]) {
    assert(argc >= 6);

    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::string h5file(argv[1]);
    std::string h5dataset(argv[2]);
    uint64_t transferSize = boost::lexical_cast< uint64_t >(argv[3]);
    uint64_t totalSize = boost::lexical_cast< uint64_t >(argv[4]);

    std::vector< std::string > h5parameters;
    for (int i=5; i<argc; i++)
        h5parameters.push_back(argv[i]);

    struct timeval start, end;

    h5reader loader(h5file, h5dataset, h5parameters, transferSize, totalSize);
    
    iobench::stats rstats;

    gettimeofday(&start, NULL);

    std::vector<int> buffer;
    uint64_t loaded_bytes = 0;
    double sum_diff_ms = 0.;
    struct timeval it_start, it_end;
    while( !loader.eof() ) {
        gettimeofday(&it_start, NULL);
        loader.readblock(buffer);
        gettimeofday(&it_end, NULL);
        double diff_s = static_cast<double>((1000 * (it_end.tv_sec - it_start.tv_sec))
                + ((it_end.tv_usec - it_start.tv_usec) / 1000)) / 1000;

        const double mb = static_cast<double>(buffer.size() * sizeof(int))/1024/1024;
        const double ops = 1;
        rstats.record(diff_s, mb, 1);
    }

    gettimeofday(&end, NULL);

    rstats.compute_stats(rank, size);
    if (rank==0)
        rstats.print(std::cout);

    MPI_Finalize();
    return 0;
}
