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
 
    std::vector< std::string > dataset;
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

    std::vector< int > buffer;

    int i=0;
    while( !loader.eof() ) {
       loader.iterateOverSynapsesFromFiles( buffer );       
       BOOST_CHECK_EQUAL(n_readSynapses, std::min(fixed_num_syns*i,128));
       i++;
    }
}


