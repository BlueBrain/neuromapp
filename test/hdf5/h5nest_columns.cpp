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

#define BOOST_TEST_MODULE hd5compoundtest
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <algorithm>

#include <mpi.h>

#include "utils/error.h"

#include "test/tools/mpi_helper.h"

#include "hdf5/h5reader.h"
#include "hdf5/data/helper.h"

BOOST_AUTO_TEST_CASE(open_file)
{
    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    unsigned long long fixed_num_syns=524288;
 
    std::vector< std::string > datasets;
    datasets.push_back("target");     
    h5reader loader(hdf5::testdata_compound(), "syn",
       datasets,
      fixed_num_syns);

    size_t num_syns = loader.size();

    BOOST_CHECK_EQUAL(num_syns, 126);
}

BOOST_AUTO_TEST_CASE(open_colums)
{
    uint64_t transferSize=20;

    std::vector< std::string > datasets;
    datasets.push_back("target");
    datasets.push_back("delay");
    datasets.push_back("TauRec");

    h5reader loader(hdf5::testdata_compound(), "syn",
                datasets,
                transferSize);

    std::vector< int > buffer(2*transferSize*datasets.size(), std::numeric_limits<int>::min());
    loader.readblock( buffer );
    BOOST_CHECK_EQUAL( buffer.size(), transferSize*datasets.size() );
}


BOOST_AUTO_TEST_CASE(open_parameter_values)
{
    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    uint64_t fixed_num_syns=20;
    
    std::vector< std::string > datasets;
    datasets.push_back("target");
    datasets.push_back("delay");
    datasets.push_back("weight");
    datasets.push_back("U0");
    datasets.push_back("TauRec");
    datasets.push_back("TauFac");
      
    h5reader loader(hdf5::testdata_compound(), "syn",
            datasets,
      fixed_num_syns);
    const uint64_t dataset_size = loader.size();
    BOOST_CHECK_EQUAL(126, dataset_size);

    uint64_t read_size = 0;
    uint64_t accu_targets = 0;

    const int entries_per_row = datasets.size();
    while( !loader.eof() ) {
       std::vector< int > buffer(2*fixed_num_syns*datasets.size(), std::numeric_limits<int>::min());
       loader.readblock( buffer );
       BOOST_CHECK_MESSAGE(buffer.size() % entries_per_row == 0, buffer.size() << " unvalid buffer size");
       BOOST_CHECK_MESSAGE(buffer.size() / entries_per_row <= fixed_num_syns, "unvalid buffer size");

       for (int j=0; j<buffer.size()/entries_per_row; j++) {
           const int target = buffer[j*entries_per_row];
           accu_targets += target;
           const float delay = *reinterpret_cast<float*>(&buffer[j*entries_per_row+1]);
           const float weight = *reinterpret_cast<float*>(&buffer[j*entries_per_row+2]);
           const float U0 = *reinterpret_cast<float*>(&buffer[j*entries_per_row+3]);
           const float TauRec = *reinterpret_cast<float*>(&buffer[j*entries_per_row+4]);
           const float TauFac = *reinterpret_cast<float*>(&buffer[j*entries_per_row+5]);

           BOOST_CHECK_CLOSE(target, delay+126, 0.000001);
           BOOST_CHECK_CLOSE(target, weight-126, 0.000001);
           BOOST_CHECK_CLOSE(target, U0+252, 0.000001);
           BOOST_CHECK_CLOSE(target, TauRec+378, 0.000001);
           BOOST_CHECK_CLOSE(target, TauFac+504, 0.000001);
       }
       read_size += buffer.size()/entries_per_row;
    }

    uint64_t total_read_size;
    MPI_Reduce(&read_size, &total_read_size, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    uint64_t total_accu_targets;
    MPI_Reduce(&accu_targets, &total_accu_targets, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank==0) {
        BOOST_CHECK_EQUAL(total_read_size, 126);
        BOOST_CHECK_EQUAL(total_accu_targets, 71379);
    }
}


