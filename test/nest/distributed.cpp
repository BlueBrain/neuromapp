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

#define BOOST_TEST_MODULE SynapseTest
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <mpi.h>

#include "utils/error.h"

#include "nest/mpi_manager.h"

#include "coreneuron_1.0/common/data/helper.h" // common functionalities

#include "test/tools/mpi_helper.h"


BOOST_AUTO_TEST_CASE(nest_distri_mpi)
{
    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //test communication with constant send_buffer size
    std::vector< int > displacements(num_processes);
    int send_buffer_size = 4;

    int recv_buffer_size = send_buffer_size * num_processes;

    std::vector< uint_t > send_buffer(send_buffer_size);
    std::vector< uint_t > recv_buffer(recv_buffer_size);

    for (int i=0; i<send_buffer_size; i++)
        send_buffer[i] = i+send_buffer_size*rank;

    nest::mpi_manager::communicate( send_buffer, recv_buffer, displacements, send_buffer_size, recv_buffer_size);

    BOOST_CHECK_EQUAL(send_buffer_size, 4);
    BOOST_CHECK_EQUAL(recv_buffer_size, 4*num_processes);

    //values should be in row
    for (int i=0; i<recv_buffer_size; i++)
        BOOST_CHECK_EQUAL(recv_buffer[i], i);

    //test communication with variable send_buffer size
    if (rank==0) {
        //send_buffer_size++;
        send_buffer.push_back(999);
    }

    nest::mpi_manager::communicate( send_buffer, recv_buffer, displacements, send_buffer_size, recv_buffer_size);

    //send_buffer_size should be the maximum value of all send_buffer sizes
    BOOST_CHECK_EQUAL(send_buffer_size, 5);
    BOOST_CHECK_EQUAL(recv_buffer_size, 5*num_processes);


    for (int r=0; r<num_processes; r++)
        for (int i=0; i<4; i++)
            BOOST_CHECK_EQUAL(recv_buffer[r*5+i], r*4+i);

    BOOST_CHECK_EQUAL(recv_buffer[4], 999);

    //empty entries should contain zeros
    for (int r=1; r<num_processes; r++)
        BOOST_CHECK_EQUAL(recv_buffer[5*r+4], 0);
}



