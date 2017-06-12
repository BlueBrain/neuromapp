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

#define BOOST_TEST_MODULE replibBenchTEST

#include <stdio.h>

#include <boost/test/unit_test.hpp>

#include "replib/benchmark.h"
#include "utils/argv_data.h"


BOOST_AUTO_TEST_CASE(replib_test){
    std::string s[16]={"binary", "-w", "rnd1b", "-o", "./currents.bbp", "-c", "1024", "-s",
            "5", "-r", "15", "-t", "10", "-v", "-b", "mpiio"};

    int narg=sizeof(s)/sizeof(s[0]);
    mapp::argv_data A(s,s+narg);
    int argc=A.argc();
    char * const *argv=A.argv();

    replib::benchmark b(argc, argv);
    b.run_benchmark();

    BOOST_CHECK_EQUAL(b.success(), true);
}


