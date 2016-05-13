/*
 * Neuromapp - spike.cpp, Copyright (c), 2015,
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
 * @file neuromapp/test/coreneuron_1.0/event_passing/spike.cpp
 *  Test on the Spike Miniapp.
 */

#define BOOST_TEST_MODULE SpikeTest
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <numeric>

#include "coreneuron_1.0/common/data/helper.h"
#include "coreneuron_1.0/event_passing/spike/algos.hpp"
#include "coreneuron_1.0/event_passing/spike/spike_interface.h"
#include "utils/error.h"
namespace bfs = ::boost::filesystem;

//Performs MPI init/finalize
#include "test/tools/mpi_helper.h"

/**
 * tests the create_spike_type() function.
 * checks that this data type can be freed using
 * MPI_Type_free
 */
BOOST_AUTO_TEST_CASE(create_spike_type_test){
    MPI_Datatype spike = create_spike_type();
    MPI_Type_free(&spike);
}


/**
 * test that the set displ function works
 * set_displ iterates over the nin vector and outputs
 * it's displacements into the displ vector
 *
 * displ is the sum of nin (up to but not including current index)
 * for nin{1, 2, 3, 4} -> displ{0, 1, 3, 5}
 * for nin{6, 2, 5, 1} -> displ{0, 6, 8, 13}
 * etc.
 */
BOOST_AUTO_TEST_CASE(displ_test){
    int size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    spike::spike_interface interface(size);

    for(int i = 0; i < size; ++i){
        interface.nin_[i] = rand()%5;
    }

    set_displ(interface);
    int sum = 0;
    for(int i = 0; i < size; ++i){
        BOOST_CHECK(interface.displ_[i] == sum);
        sum += interface.nin_[i];
    }
}

/**
 * for queueing::pool and spike::environment
 * test that run sim function results in the expected end state
 * for the case of blocking spike exchange (random # of input_presyns)
 *
BOOST_AUTO_TEST_CASE(blocking_spike_exchange){
    int size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL) + rank);

    int nspike = 100;
    int nlocal = 100;
    int simtime = 100;
    int netcons = 5;
    int numOut = 2;
    int numIn = numOut * (size - 1);



}*/
