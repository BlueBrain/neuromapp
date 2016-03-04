/*
 * Neuromapp - args.cpp, Copyright (c), 2015,
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
 * @file neuromapp/test/keyvalue/args.cpp
 *  Test on the key/value store miniapp arguments
 */

#define BOOST_TEST_MODULE KeyValueTest
#include <boost/test/unit_test.hpp>
#include "keyvalue/utils/argument.h"
#include "utils/argv_data.h"

//Performs MPI init/finalize
#include "test/tools/mpi_helper.h"

BOOST_AUTO_TEST_CASE(args_constructor_default_test){
    keyvalue::argument a;
    float st = 1.0;
    float md = 0.1;
    float dt = 0.025;
    int procs = 1;
    int threads = 1;

    MPI_Comm_size(MPI_COMM_WORLD, &procs); 

    #pragma omp parallel
    {
        threads = omp_get_num_threads();
    }

    BOOST_CHECK_EQUAL(a.procs(), procs);
    BOOST_CHECK_EQUAL(a.threads(), threads);
    BOOST_CHECK_EQUAL(a.backend(), "map");
    BOOST_CHECK_EQUAL(a.async(), false);
    BOOST_CHECK_EQUAL(a.flash(), false);
    BOOST_CHECK_EQUAL(a.usecase(), 1);
    BOOST_CHECK_EQUAL(a.st(), st);
    BOOST_CHECK_EQUAL(a.md(), md);
    BOOST_CHECK_EQUAL(a.dt(), dt);
    BOOST_CHECK_EQUAL(a.cg(), 1);
    BOOST_CHECK_EQUAL(a.voltages_size(), a.usecase()*4096/2.5*350);
}


BOOST_AUTO_TEST_CASE(args_constructor_test){
    keyvalue::argument a;
    float st = 0.5;
    float md = 0.2;
    float dt = 0.01;
    int voltages_size = 2*4096/2.5;

    a.procs() = 4;
    a.threads() = 2;
    a.backend() = "skv";
    a.async() = true;
    a.flash() = true;
    a.usecase() = 2;
    a.st() = st;
    a.md() = md;
    a.dt() = dt;
    a.cg() = 4;
    a.voltages_size() = voltages_size;

    BOOST_CHECK_EQUAL(a.procs(), 4);
    BOOST_CHECK_EQUAL(a.threads(), 2);
    BOOST_CHECK_EQUAL(a.backend(), "skv");
    BOOST_CHECK_EQUAL(a.async(), true);
    BOOST_CHECK_EQUAL(a.flash(), true);
    BOOST_CHECK_EQUAL(a.usecase(), 2);
    BOOST_CHECK_EQUAL(a.st(), st);
    BOOST_CHECK_EQUAL(a.md(), md);
    BOOST_CHECK_EQUAL(a.dt(), dt);
    BOOST_CHECK_EQUAL(a.cg(), 4);
    BOOST_CHECK_EQUAL(a.voltages_size(), voltages_size);
}

BOOST_AUTO_TEST_CASE(args_constructor_argv_test){
    std::string s[15]={"binary", "-b", "skv", "-st", "0.2", "-md", "0.05", "-dt", "0.005", "-cg", "8", "-uc", "3", "-a", "-f"};
    int narg=sizeof(s)/sizeof(s[0]);

    mapp::argv_data A(s,s+narg);

    int argc=A.argc();
    char * const *argv=A.argv();

    keyvalue::argument a(argc, argv);

    a.procs() = 1;
    a.threads() = 1;

    BOOST_CHECK_EQUAL(a.procs(), 1);
    BOOST_CHECK_EQUAL(a.threads(), 1);
    BOOST_CHECK_EQUAL(a.backend(), "skv");
    BOOST_CHECK_EQUAL(a.async(), true);
    BOOST_CHECK_EQUAL(a.flash(), true);
    BOOST_CHECK_EQUAL(a.usecase(), 3);
    BOOST_CHECK_EQUAL(a.st(), (float) 0.2);
    BOOST_CHECK_EQUAL(a.md(), (float) 0.05);
    BOOST_CHECK_EQUAL(a.dt(), (float) 0.005);
    BOOST_CHECK_EQUAL(a.cg(), 8);
    BOOST_CHECK_EQUAL(a.voltages_size(), a.usecase()*4096/2.5*350);
}

