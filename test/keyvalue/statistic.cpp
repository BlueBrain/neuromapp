/*
 * Neuromapp - statistic.cpp, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
 * timothee.ewart@epfl.ch,
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
 * @file neuromapp/test/keyvalue/statistic.cpp
 *  Test on the key/value store miniapp statistics
 */

#define BOOST_TEST_MODULE KeyValueTestMPI

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/cxx11/iota.hpp> // for fun
#include "keyvalue/utils/tools.h" // it starts the MPI, see the associated pattern
#include "keyvalue/utils/argument.h"
#include "keyvalue/utils/statistic.h"

//Performs MPI init/finalize
#include "test/tools/mpi_helper.h"

BOOST_AUTO_TEST_CASE(statistic_constructors_default_test){
    keyvalue::statistic s;
    BOOST_CHECK_CLOSE(s.ops(), (s.args().st()/s.args().dt())*s.args().cg(), 0.00001);
    BOOST_CHECK_CLOSE(s.size(), (s.args().voltages_size()*sizeof(double))/(1024.*1024.), 0.00001);
    BOOST_CHECK_EQUAL(s.iops(), 0.);
    BOOST_CHECK_EQUAL(s.mbw(), 0.);
}

BOOST_AUTO_TEST_CASE(statistic_constructors_test){
    keyvalue::argument a;
    unsigned int num_ops = 10;
    double size = 100.0;
    keyvalue::statistic s(a, num_ops, size);
    BOOST_CHECK_EQUAL(s.ops(), num_ops);
    BOOST_CHECK_CLOSE(s.size(), size/(1024.*1024.), 0.00001);
    BOOST_CHECK_EQUAL(s.iops(), 0.);
    BOOST_CHECK_EQUAL(s.mbw(), 0.);
}

BOOST_AUTO_TEST_CASE(accumulate_mpi_test){
    int numprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size(100);
    std::vector<double> v(size+1,0.);
    boost::algorithm::iota(v.begin(),v.end(),0); //0,1,2 ...
    double tmp = keyvalue::accumulate(v.begin(), v.end(), 0.); // MPI is inside, reduction on 0, only
    if (rank == 0)
        BOOST_CHECK_EQUAL(tmp, size*(size+1)/2*numprocs);
}

BOOST_AUTO_TEST_CASE(reduce_mpi_test){
    int numprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double x = rank;
    double tmp = keyvalue::reduce(x); // MPI is inside, reduction on 0, only
    if (rank == 0)
        BOOST_CHECK_EQUAL(tmp, (numprocs-1.)*numprocs/2.);
}

BOOST_AUTO_TEST_CASE(reduce_it_mpi_test){
    int numprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size(100);
    std::vector<double> v(size+1,0.);
    boost::algorithm::iota(v.begin(),v.end(),0); //0,1,2 ...
    // MPI is inside, reduction on 0, only
    keyvalue::reduce<std::vector<double>::iterator, double>(v.begin(), v.end());
    if (rank == 0) {
        for (unsigned int i = 0; i < v.size(); i++) {
            BOOST_CHECK_EQUAL(v[i], (double) i * numprocs);
        }
    }
}

BOOST_AUTO_TEST_CASE(statistic_process_loop_test){
    int numprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    keyvalue::argument a;
    unsigned int num_ops = 10;
    double size = 20.0 * sizeof(double);
    unsigned int vsize = 10;
    std::vector<double> v(vsize);
    boost::algorithm::iota(v.begin(),v.end(),0); //0,1,2 ...
    keyvalue::statistic s(a, num_ops, size, v);
    s.process();
    size = size / (1024 * 1024);
    double time = (vsize-1.) * vsize / 2.;
    double iops = (double) num_ops / time;
    double mbw = (size * num_ops) / time;
    if (rank == 0) {
        BOOST_CHECK_EQUAL(s.iops(), iops*numprocs);
        BOOST_CHECK_EQUAL(s.mbw(), mbw*numprocs);
    }
}

BOOST_AUTO_TEST_CASE(statistic_process_task_test){
    int numprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    keyvalue::argument a;
    a.taskdeps() = true;
    unsigned int num_ops = 10;
    double size = 20.0 * sizeof(double);
    unsigned int vsize = 2;
    std::vector<double> v(vsize);
    v[0] = 2.5;
    v[1] = 3.0;
    keyvalue::statistic s(a, num_ops, size, v);
    s.process();
    size = size / (1024 * 1024);
    double time = v[1] - v[0];
    double iops = (double) num_ops / time;
    double mbw = (size * num_ops) / time;
    if (rank == 0) {
        BOOST_CHECK_EQUAL(s.iops(), iops*numprocs);
        BOOST_CHECK_EQUAL(s.mbw(), mbw*numprocs);
    }
}
