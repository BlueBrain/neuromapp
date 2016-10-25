/*
 * Neuromapp - io-omp.cpp, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * @file neuromapp/test/iobench/bench.cpp
 *  General test on the iobench miniapp
 */

#ifdef IO_MPI
#define BOOST_TEST_MODULE IOBenchTestMPI
#else
#define BOOST_TEST_MODULE IOBenchTest
#endif

#include <stdio.h>

#include <boost/test/unit_test.hpp>

#include "iobench/benchmark.h"
#include "utils/argv_data.h"


BOOST_AUTO_TEST_CASE(iobench_test){

    std::string s[23]={"binary", "-b", "map", "-n", "128", "-i", "3", "-s", "1", "-k", "32",
            "-v", "256", "-c", "0", "-r", "1", "-w", "1", "--rrd", "1", "--rwr", "1"};

    int narg=sizeof(s)/sizeof(s[0]);
    mapp::argv_data A(s,s+narg);
    int argc=A.argc();
    char * const *argv=A.argv();

    iobench::benchmark b(argc, argv);
    b.createData();
    b.run();

    BOOST_CHECK_EQUAL(b.success(), true);
}


