/*
 * Neuromapp - stats.cpp, Copyright (c), 2015,
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
 * @file neuromapp/test/iobench/stats.cpp
 *  Test on the iobench miniapp statistics
 */

#ifdef IO_MPI
#define BOOST_TEST_MODULE IOBenchTestMPI
#else
#define BOOST_TEST_MODULE IOBenchTest
#endif

#include <boost/test/unit_test.hpp>
#include "iobench/utils/stats.h"

#ifdef IO_MPI
//Performs MPI init/finalize
#include "test/tools/mpi_helper.h"
#endif

BOOST_AUTO_TEST_CASE(stats_constructor_default_test){
    iobench::stats st;
    BOOST_CHECK_CLOSE(st.mb(), 0.0, 0.00001);
    BOOST_CHECK_EQUAL(st.ops(), 0);
    BOOST_CHECK_CLOSE(st.avgBW(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.stddevBW(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.stderrBW(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.avgIOPS(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.stddevIOPS(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.stderrIOPS(), 0.0, 0.00001);
}

#ifdef IO_MPI
BOOST_AUTO_TEST_CASE(stats_killer_mpi_test){
    iobench::stats st;

    double times[8] = {3.5, 4.8, 2.9, 6.7, 3.9, 4.2, 5.3, 6.4};
    double mb = 1024;
    double ops = 2048;

    double avgb = 0.0, avgi;
    st.mb() = mb;
    st.ops() = ops;
    for (int i = 0; i < 8; i++) {
        st.record(times[i]);
        avgb += mb / times[i];
        avgi += ops / times[i];
    }
    avgb /= 8;
    avgi /= 8;
    // Stddev and stderr are 0.0 because all ranks compute the same avg
    // FIXME: introduce variability accross ranks and compute this properly
    double stddevb = 0.0, stddevi = 0.0;

    int mpi_size, mpi_rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    st.compute_stats(mpi_rank, mpi_size);

    if( mpi_rank == 0) {
        BOOST_CHECK_EQUAL(st.ops(), ops);
        BOOST_CHECK_EQUAL(st.mb(), mb);
        BOOST_CHECK_CLOSE(st.avgBW(), avgb, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevBW(), stddevb, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrBW(), stderrb, 0.00001);
        BOOST_CHECK_CLOSE(st.avgIOPS(), avgi, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevIOPS(), stddevi, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrIOPS(), stderri, 0.00001);

    }
}
#else
BOOST_AUTO_TEST_CASE(stats_killer_test){
    iobench::stats st;

    double times[8] = {3.5, 4.8, 2.9, 6.7, 3.9, 4.2, 5.3, 6.4};
    double mb = 1024;
    double ops = 2048;

    double avgb = 0.0, avgi;
    st.mb() = mb;
    st.ops() = ops;
    for (int i = 0; i < 8; i++) {
        st.record(times[i]);
        avgb += mb / times[i];
        avgi += ops / times[i];
    }
    avgb /= 8;
    avgi /= 8;
    double stddevb = 0.0, stddevi = 0.0;
    for (int i = 0; i < 8; i++) {
        stddevb += ((mb/times[i]) - avgb) * ((mb/times[i]) - avgb);
        stddevi += ((ops/times[i]) - avgi) * ((ops/times[i]) - avgi);
    }
    stddevb = sqrt(1.0 / 8 * stddevb);
    stddevi = sqrt(1.0 / 8 * stddevi);
    double stderrb = stddevb / sqrt(8);
    double stderri = stddevi / sqrt(8);

    int mpi_size = -1, mpi_rank = 0;

    st.compute_stats(mpi_rank, mpi_size);

    if( mpi_rank == 0) {
        BOOST_CHECK_EQUAL(st.ops(), ops);
        BOOST_CHECK_EQUAL(st.mb(), mb);
        BOOST_CHECK_CLOSE(st.avgBW(), avgb, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevBW(), stddevb, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrBW(), stderrb, 0.00001);
        BOOST_CHECK_CLOSE(st.avgIOPS(), avgi, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevIOPS(), stddevi, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrIOPS(), stderri, 0.00001);

    }
}
#endif
