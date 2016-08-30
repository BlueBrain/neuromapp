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
 * @file neuromapp/test/iobench/args.cpp
 *  Test on the iobench miniapp arguments
 */

#ifdef IO_MPI
#define BOOST_TEST_MODULE IOBenchTestMPI
#else
#define BOOST_TEST_MODULE IOBenchTest
#endif

#include <boost/test/unit_test.hpp>
#include "iobench/utils/args.h"
#include "utils/argv_data.h"

#ifdef IO_MPI
//Performs MPI init/finalize
#include "test/tools/mpi_helper.h"
#endif

BOOST_AUTO_TEST_CASE(args_constructor_default_test){
    iobench::args a;

    int             procs = -1;
    int             rank = 0;
    unsigned int    threads = 1;
    unsigned int    npairs = 1024;
    unsigned int    key_size = 32;
    unsigned int    value_size = 1024;
    std::string     backend ("map") ;
    bool            read = true;
    bool            write = true;
    bool            compress = false;
    bool            rnd_rd = true;
    bool            rnd_wr = true;
    unsigned int    niter = 1;
    unsigned int    skip = 0;

#ifdef IO_MPI
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

    #pragma omp parallel
    {
#ifdef _OPENMP
        threads = omp_get_num_threads();
#else
        threads = 1;
#endif
    }

    BOOST_CHECK_EQUAL(a.procs(), procs);
    BOOST_CHECK_EQUAL(a.rank(), rank);
    BOOST_CHECK_EQUAL(a.threads(), threads);
    BOOST_CHECK_EQUAL(a.npairs(), npairs);
    BOOST_CHECK_EQUAL(a.keysize(), key_size);
    BOOST_CHECK_EQUAL(a.valuesize(), value_size);
    BOOST_CHECK_EQUAL(a.backend(), backend);
    BOOST_CHECK_EQUAL(a.read(), read);
    BOOST_CHECK_EQUAL(a.write(), write);
    BOOST_CHECK_EQUAL(a.compress(), compress);
    BOOST_CHECK_EQUAL(a.rnd_rd(), rnd_rd);
    BOOST_CHECK_EQUAL(a.rnd_wr(), rnd_wr);
    BOOST_CHECK_EQUAL(a.niter(), niter + skip);
    BOOST_CHECK_EQUAL(a.skip(), skip);
}


BOOST_AUTO_TEST_CASE(args_constructor_test){
    iobench::args a;

    unsigned int    npairs = 512;
    unsigned int    key_size = 64;
    unsigned int    value_size = 128;
    std::string     backend ("ldb") ;
    bool            read = false;
    bool            write = false;
    bool            compress = true;
    bool            rnd_rd = false;
    bool            rnd_wr = false;
    unsigned int    niter = 3;
    unsigned int    skip = 2;

    a.npairs() = npairs;
    a.keysize() = key_size;
    a.valuesize() = value_size;
    a.backend() = backend;
    a.read() = read;
    a.write() = write;
    a.compress() = compress;
    a.rnd_rd() = rnd_rd;
    a.rnd_wr() = rnd_wr;
    a.niter() = niter + skip;
    a.skip() = skip;


    BOOST_CHECK_EQUAL(a.npairs(), npairs);
    BOOST_CHECK_EQUAL(a.keysize(), key_size);
    BOOST_CHECK_EQUAL(a.valuesize(), value_size);
    BOOST_CHECK_EQUAL(a.backend(), backend);
    BOOST_CHECK_EQUAL(a.read(), read);
    BOOST_CHECK_EQUAL(a.write(), write);
    BOOST_CHECK_EQUAL(a.compress(), compress);
    BOOST_CHECK_EQUAL(a.rnd_rd(), rnd_rd);
    BOOST_CHECK_EQUAL(a.rnd_wr(), rnd_wr);
    BOOST_CHECK_EQUAL(a.niter(), niter + skip);
    BOOST_CHECK_EQUAL(a.skip(), skip);
}

BOOST_AUTO_TEST_CASE(args_constructor_argv_test){
    std::string s[23]={"binary", "-b", "cassandra", "-n", "2048", "-i", "7", "-s", "4", "-k", "256",
            "-v", "512", "-c", "1", "-r", "1", "-w", "0", "--rrd", "0", "--rwr", "1"};

    int narg=sizeof(s)/sizeof(s[0]);

    mapp::argv_data A(s,s+narg);

    int argc=A.argc();
    char * const *argv=A.argv();

    iobench::args a(argc, argv);

    a.procs() = 1;
    a.threads() = 1;

    BOOST_CHECK_EQUAL(a.procs(), 1);
    BOOST_CHECK_EQUAL(a.threads(), 1);
    BOOST_CHECK_EQUAL(a.npairs(), 2048);
    BOOST_CHECK_EQUAL(a.keysize(), 256);
    BOOST_CHECK_EQUAL(a.valuesize(), 512);
    BOOST_CHECK_EQUAL(a.backend(), "cassandra");
    BOOST_CHECK_EQUAL(a.read(), true);
    BOOST_CHECK_EQUAL(a.write(), false);
    BOOST_CHECK_EQUAL(a.compress(), true);
    BOOST_CHECK_EQUAL(a.rnd_rd(), false);
    BOOST_CHECK_EQUAL(a.rnd_wr(), true);
    BOOST_CHECK_EQUAL(a.niter(), 7 + 4);
    BOOST_CHECK_EQUAL(a.skip(), 4);
}

