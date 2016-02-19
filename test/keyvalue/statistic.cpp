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
 *  Test on the key/value store miniapp arguments
 */

#define BOOST_TEST_MODULE KeyValueTestMPI

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/cxx11/iota.hpp> // for fun
#include "keyvalue/utils/tools.h" // it starts the MPI, see the associated pattern
#include "keyvalue/utils/argument.h"
#include "keyvalue/utils/statistic.h"

struct MPIInitializer {
    MPIInitializer(){
        MPI::Init();
    }
    ~MPIInitializer(){
        MPI::Finalize();
    }
};

//performs mpi initialization/finalize
BOOST_GLOBAL_FIXTURE(MPIInitializer);

BOOST_AUTO_TEST_CASE(statistic_constructors_test){
    keyvalue::statistic s;
    BOOST_CHECK_EQUAL(s.iops(), 0.);
    BOOST_CHECK_EQUAL(s.mbw(), 0.);
}

BOOST_AUTO_TEST_CASE(accumulate_mpi_test){
    int numprocs = MPI::COMM_WORLD.Get_size();
    int size(100);
    std::vector<double> v(size+1,0.);
    boost::algorithm::iota(v.begin(),v.end(),0); //0,1,2 ...
    double tmp = keyvalue::accumulate(v.begin(), v.end(), 0.); // MPI is inside, reduction on 0, only
    if( MPI::COMM_WORLD.Get_rank() == 0)
        BOOST_CHECK_EQUAL(tmp, size*(size+1)/2*numprocs);
}

