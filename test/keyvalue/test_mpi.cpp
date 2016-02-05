/*
 * Neuromapp - test-args.cpp, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
 * timothee.ewart@epfl.ch,
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
 * @file neuromapp/test/keyvalue/test_mpi.cpp
 *  Test on the key/value store miniapp arguments
 */

#define BOOST_TEST_MODULE KeyValueTestMPI

#include <boost/test/unit_test.hpp>
#include "keyvalue/utils/tools.h" // it starts the MPI, see the associated pattern
#include "keyvalue/utils/argument.h"
#include "keyvalue/utils/statistic.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;


BOOST_AUTO_TEST_CASE(statistic_constructors_test){
    {
        keyvalue::statistic s;
        BOOST_CHECK_EQUAL(s.iops(), 0.);
        BOOST_CHECK_EQUAL(s.mbw(), 0.);
    }

    {
        keyvalue::statistic s;
        BOOST_CHECK_EQUAL(s.iops(), 0.);
        BOOST_CHECK_EQUAL(s.mbw(), 0.);
    }
}

BOOST_AUTO_TEST_CASE(accumulate_mpi_test){
    std::vector<double> v(100,1);
    double tmp = keyvalue::utils::accumulate(v.begin(), v.end(), 0.); // MPI is inside
    BOOST_CHECK_EQUAL(tmp, 100 );
}

