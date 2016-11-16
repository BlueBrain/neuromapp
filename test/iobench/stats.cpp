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
    BOOST_CHECK_CLOSE(st.avgMB(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.stddevMB(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.stddevMB(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.avgOPS(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.stddevOPS(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.stddevOPS(), 0.0, 0.00001);
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
    double mb = 1024.;
    double ops = 2048.;

    double avgb = 0.0, avgi = 0.0;
    for (int i = 0; i < 8; i++) {
        st.record(times[i], mb, ops);
        avgb += mb / times[i];
        avgi += ops / times[i];
    }
    avgb /= 8.;
    avgi /= 8.;
    // Stddev and stderr are 0.0 because all ranks compute the same avg
    // (except when there is only 1 rank)
    // FIXME: introduce variability across ranks and compute this properly
    double stddev = 0.0, stderr = 0.0, stddevb = 0.0, stderrb = 0.0, stddevi = 0.0, stderri = 0.0;

    int mpi_size, mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    st.compute_stats(mpi_rank, mpi_size);

    if (mpi_size == 1) {
        // Compute the correct stddev and stderr for bw and iops
        for (int i = 0; i < 8; i++) {
            stddevb += ((mb/times[i]) - avgb) * ((mb/times[i]) - avgb);
            stddevi += ((ops/times[i]) - avgi) * ((ops/times[i]) - avgi);
        }
        stddevb = sqrt(1.0 / 8 * stddevb);
        stddevi = sqrt(1.0 / 8 * stddevi);
        stderrb = stddevb / sqrt(8);
        stderri = stddevi / sqrt(8);
    }

    if (mpi_rank == 0) {
        BOOST_CHECK_CLOSE(st.avgMB_per_cycle(), mb*mpi_size, 0.00001);
        BOOST_CHECK_CLOSE(st.avgOPS_per_cycle(), ops*mpi_size, 0.00001);
        BOOST_CHECK_CLOSE(st.avgMB(), mb, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevMB(), stddev, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrMB(), stderr, 0.00001);
        BOOST_CHECK_CLOSE(st.avgOPS(), ops, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevOPS(), stddev, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrOPS(), stderr, 0.00001);
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

    std::vector< double > times(8);
    times[0] = 3.5;
    times[1] = 4.8;
    times[2] = 2.9;
    times[3] = 6.7;
    times[4] = 3.9;
    times[5] = 4.2;
    times[6] = 5.3;
    times[7] = 6.4;

    double mb = 1024;
    double ops = 2048;

    double avgb = 0.0, avgi=0.0, avgm=0.0, avgo=0.0;
    for (int i = 0; i < 8; i++) {
        st.record(times[i], mb, ops);
        avgb += mb / times[i];
        avgi += ops / times[i];
        avgm += mb;
        avgo += ops;

    }
    avgb /= 8;
    avgi /= 8;
    avgm /= 8;
    avgo /= 8;

    double stddevb = 0.0, stddevi = 0.0, stddevm=0.0, stddevo=0.0;
    for (int i = 0; i < 8; i++) {
        stddevb += ((mb/times[i]) - avgb) * ((mb/times[i]) - avgb);
        stddevi += ((ops/times[i]) - avgi) * ((ops/times[i]) - avgi);
        stddevm += (mb - avgm) * (mb - avgm);
        stddevo += (ops - avgo) * (ops - avgo);
    }
    stddevb = sqrt(1.0 / 8 * stddevb);
    stddevi = sqrt(1.0 / 8 * stddevi);
    stddevm = sqrt(1.0 / 8 * stddevm);
    stddevo = sqrt(1.0 / 8 * stddevo);
    double stderrb = stddevb / sqrt(8);
    double stderri = stddevi / sqrt(8);
    double stderrm = stddevm / sqrt(8);
    double stderro = stddevo / sqrt(8);

    int mpi_size = -1, mpi_rank = 0;

    st.compute_stats(mpi_rank, mpi_size);

    if (mpi_rank == 0) {
        BOOST_CHECK_CLOSE(st.total_time(), std::accumulate(times.begin(), times.end(), 0.0), 0.00001);
        BOOST_CHECK_CLOSE(st.total_mb(), mb*8, 0.00001);
        BOOST_CHECK_CLOSE(st.total_ops(), ops*8, 0.00001);

        BOOST_CHECK_CLOSE(st.avgOPS_per_cycle(), ops, 0.00001);
        BOOST_CHECK_CLOSE(st.avgMB_per_cycle(), mb, 0.00001);
        BOOST_CHECK_CLOSE(st.avgMB(), avgm, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevMB(), stddevm, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrMB(), stderrm, 0.00001);
        BOOST_CHECK_CLOSE(st.avgOPS(), avgo, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevOPS(), stddevo, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrOPS(), stderro, 0.00001);
        BOOST_CHECK_CLOSE(st.avgBW(), avgb, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevBW(), stddevb, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrBW(), stderrb, 0.00001);
        BOOST_CHECK_CLOSE(st.avgIOPS(), avgi, 0.00001);
        BOOST_CHECK_CLOSE(st.stddevIOPS(), stddevi, 0.00001);
        BOOST_CHECK_CLOSE(st.stderrIOPS(), stderri, 0.00001);

    }
}
#endif
