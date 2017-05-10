/*
 * Neuromapp - stats.cpp, Copyright (c), 2015,
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
 * @file neuromapp/test/replib/stats.cpp
 *  Test on the replib miniapp statistics
 */

#define BOOST_TEST_MODULE replibStatsTEST

#include <vector>
#include <numeric>

#include <boost/test/unit_test.hpp>
#include "replib/utils/statistics.h"
#include "replib/utils/config.h"

//Performs MPI init/finalize
#include "test/tools/mpi_helper.h"

struct double_int {
    double d_; // rank's bandwidth
    int i_;    // rank ID
};

BOOST_AUTO_TEST_CASE(stats_constructor_default_test){
    replib::config c;
    replib::statistics st(c, 0, std::vector<double>());

    BOOST_CHECK_EQUAL(st.bytes(), 0);
    BOOST_CHECK_CLOSE(st.mbw(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(st.aggr_mbw(), 0.0, 0.00001);
}

BOOST_AUTO_TEST_CASE(stats_killer_mpi_test){
    replib::config c;
    c.numcells() = 512;
    c.elems_per_step() = c.numcells() * 350;

    unsigned int bytes = c.elems_per_step() * sizeof(float);

    int mpi_size, mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    double t[8] = {3.5, 4.8, 2.9, 6.7, 3.9, 4.2, 5.3, 6.4};
    std::vector<double> times;
    for (int i = 0; i < 8; i++) {
        times.push_back((t[i] + (10 - mpi_rank)) / 10000.);
    }

    replib::statistics st(c, bytes, times);

    // Compute bytes and time per rank
    double rmb = (double) bytes / (1024.*1024.);
    double rtime = std::accumulate(times.begin(), times.end(), 0.0);
    double rbw = rmb/ rtime;

    // Compute aggregated values, only rank 0 gets the result
    double time = 0.0;
    MPI_Reduce(&rtime, &time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    double mb = 0.0;
    MPI_Reduce(&rmb, &mb, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double avgb = 0.0;
    double aggrb = 0.0;
    if (mpi_rank == 0) {
        avgb = mb / time;
        aggrb = avgb * mpi_size;
    }

    // Compute max and min ranks
    double_int me, max, min;
    me.d_ = rbw;
    me.i_ = mpi_rank;
    MPI_Reduce(&me, &max, 1, MPI_DOUBLE_INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);
    MPI_Reduce(&me, &min, 1, MPI_DOUBLE_INT, MPI_MINLOC, 0, MPI_COMM_WORLD);

    replib::bw_stats max_st, min_st;
    max_st.rank_ = max.i_;
    max_st.size_ = bytes;
    max_st.time_ = 0.0;
    max_st.mbw_ = max.d_;
    min_st.rank_ = min.i_;
    min_st.size_ = bytes;
    min_st.time_ = 0.0;
    min_st.mbw_ = min.d_;

    st.process();

    if (mpi_rank == 0) {
        BOOST_CHECK_EQUAL(st.bytes(), bytes);
        BOOST_CHECK_CLOSE(st.mbw(), avgb, 0.00001);
        BOOST_CHECK_CLOSE(st.aggr_mbw(), aggrb, 0.00001);
        BOOST_CHECK_EQUAL(st.get_max().rank_, max_st.rank_);
        BOOST_CHECK_EQUAL(st.get_max().size_, max_st.size_);
        BOOST_CHECK_CLOSE(st.get_max().time_, max_st.time_, 0.00001);
        BOOST_CHECK_CLOSE(st.get_max().mbw_, max_st.mbw_, 0.00001);
        BOOST_CHECK_EQUAL(st.get_min().rank_, min_st.rank_);
        BOOST_CHECK_EQUAL(st.get_min().size_, min_st.size_);
        BOOST_CHECK_CLOSE(st.get_min().time_, min_st.time_, 0.00001);
        BOOST_CHECK_CLOSE(st.get_min().mbw_, min_st.mbw_, 0.00001);
    }
}
