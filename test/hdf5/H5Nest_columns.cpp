/*
 * Neuromapp - args.cpp, Copyright (c), 2015,
 * Till Schumann - Swiss Federal Institute of technology in Lausanne,
 * till.schumann@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/nest/synapse.cpp
 *  Test on the nest synapse module
 */

#define BOOST_TEST_MODULE SynapseDistriTest
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <algorithms>

#include <mpi.h>

#include "utils/error.h"

#include "test/tools/mpi_helper.h"

BOOST_AUTO_TEST_CASE(open_file)
{
    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    uint64_t n_readSynapses=0;
    uint64_t n_SynapsesInDatasets=0;
    uint64_t fixed_num_syns=524288;
 
    std::vector< std::string > datasets;
    datasets.push_back("target");     
    H5SynapsesLoader loader(H5NEST_COLUMN_TESTFILE, datasets,
      n_readSynapses,
      n_SynapsesInDatasets,
      fixed_num_syns);

    size_t num_syns = loader.getNumberOfSynapses();

    BOOST_CHECK_EQUAL(num_syns, 128);
}


BOOST_AUTO_TEST_CASE(open_num_syns)
{
    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    uint64_t n_readSynapses=0;
    uint64_t n_SynapsesInDatasets=0;
    uint64_t fixed_num_syns=20;
    
    std::vector< std::string > datasets;
    datasets.push_back("target");
    datasets.push_back("weight");
      
    H5SynapsesLoader loader(H5NEST_COLUMN_TESTFILE, datasets,
      n_readSynapses,
      n_SynapsesInDatasets,
      fixed_num_syns);

    uint64_t old_n_readSynapses = n_readSynapses;
    int i=1;
    while( !loader.eof() ) {
       std::vector< int > buffer(2*fixed_num_syns*datasets.size(), std::numeric_limits<int>::min());
       loader.iterateOverSynapsesFromFiles( buffer );

       BOOST_CHECK_EQUAL((n_readSynapses-old_n_readSynapses)*datasets.size(), buffer.size()));
       for (int j=0; j<buffer.size()/datasets.size(); j++) {
           BOOST_CHECK( 0 <= buffer[j] && buffer[j] <= 75000000 ); // valid target range
           BOOST_CHECK( 0.2 <= reinterpret_cast<float>(buffer[j+1]) && reinterpret_cast<float>(buffer[j+1]) <= 50. ); // valid weight range
       }
       old_n_readSynapses = n_readSynapses;
    }

    uint64_t all_n_readSynapses;
    MPI_Allreduce(&n_readSynapses,
        &all_n_readSynapses,
        1,
        MPI_UNSIGNED_LONG_LONG,
        MPI_SUM,
        MPI_COMM_WORLD);

    BOOST_CHECK_EQUAL(128, n_SynapsesInDatasets);

    if (rank==0)
        BOOST_CHECK_EQUAL(all_n_readSynapses, n_SynapsesInDatasets);
}


