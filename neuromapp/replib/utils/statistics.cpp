/*
 * Neuromapp - statistics.cpp, Copyright (c), 2015,
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
 * GNU General Public License for more details. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/replib/utils/statistics.cpp
 * \brief basic shell for statistics
 */

#include <numeric>
#include <algorithm>
#include <string>
#include <ostream>

#include "replib/utils/statistics.h"
#include "replib/utils/tools.h"

namespace replib {

struct double_int {
    double d_; // rank's bandwidth
    int i_;    // rank ID
};

/** \fun void statistics::process()
    \brief compute the miniapp statistics based on the information
    recorded while the benchmark was running
 */
void statistics::process() {
    // First, compute BW as sum(bytes) / sum(time) for all ranks
    // Add up values per rank, then reduce across ranks
    double r_time = std::accumulate(times_.begin(), times_.end(), 0.0);
    double r_mb = (bytes_ * times_.size()) / (1024. * 1024.);

    // Only rank 0 gets the result
    double time = replib::reduce(r_time);
    double mb = replib::reduce(r_mb);
    if (c_.id() == 0) {
        g_mbw_ = mb / time;
        a_mbw_ = g_mbw_ * c_.procs();
    }

    // Compute BW per rank
    double r_mbw = r_mb / r_time;

    // Find max and min rank statistics (max and min results stored in rank 0)
    double_int me;
    me.d_ = r_mbw;
    me.i_ = c_.id();
    double_int max;
    max.d_ = 0;
    max.i_ = 0;
    double_int min;
    min.d_ = 0;
    min.i_ = 0;
    // Only rank 0 will have the results
    int master_rank = 0;
    replib::max(&me, &max, master_rank);
    replib::min(&me, &min, master_rank);
    max_.rank_ = max.i_;
    max_.mbw_ = max.d_;
    min_.rank_ = min.i_;
    min_.mbw_ = min.d_;

    // Broadcast results to get the writing size of min and max ranks
    MPI_Bcast(&max_.rank_, 1, MPI_INT, master_rank, MPI_COMM_WORLD);
    MPI_Bcast(&min_.rank_, 1, MPI_INT, master_rank, MPI_COMM_WORLD);

    max_.size_ = bytes_; // Only max rank will have the right value
    min_.size_ = bytes_; // Only min rank will have the right value

    // Max and min ranks send the size of their writes
    if (max_.rank_ != master_rank && max_.rank_ == me.i_) {
        MPI_Send(&max_.size_, 1, MPI_UNSIGNED, master_rank, me.i_, MPI_COMM_WORLD);
    }
    if (min_.rank_ != master_rank && min_.rank_ == me.i_) {
        MPI_Send(&min_.size_, 1, MPI_UNSIGNED, master_rank, me.i_, MPI_COMM_WORLD);
    }

    // Rank 0 receives the values
    if (max_.rank_ != master_rank && me.i_ == master_rank) {
        MPI_Status status;
        MPI_Recv(&max_.size_, 1, MPI_UNSIGNED, max_.rank_, max_.rank_, MPI_COMM_WORLD, &status);
    }
    if (min_.rank_ != master_rank && me.i_ == master_rank) {
        MPI_Status status;
        MPI_Recv(&min_.size_, 1, MPI_UNSIGNED, min_.rank_, min_.rank_, MPI_COMM_WORLD, &status);
    }
}

/** \brief the print function */
void statistics::print(std::ostream& os) const {
    // WARNING: This relies on the fact that only rank 0 will print
    // and, actually, only rank 0 has the correct results!
    os << "Mini-app configuration:" << std::endl;
    c_.print(os);

    os << "Average bandwidth: " << g_mbw_ << " MB/s per rank" << std::endl
            << "Aggregated bandwidth: " << a_mbw_ << " MB/s" << std::endl
            << "Max bandwidth: " << max_.mbw_ << " MB/s writing " << max_.size_ / 1024.
            << " KB from rank " << max_.rank_ << std::endl
            << "Min bandwidth: " << min_.mbw_ << " MB/s writing " << min_.size_ / 1024.
            << " KB from rank " << min_.rank_ << std::endl;


    // CSV output data format:
    // miniapp_name, num_procs, writeMode, invertRanks, numCells, simulationSteps, reportingSteps,
    // avgRankBW (MB/s), aggregatedBW (MB/s), maxBW, maxBWsize, maxBWrank, minBW, minBWsize, minBWrank
    os << "RLMAPP," << c_.procs() << "," << c_.write() << "," << ( c_.invert() ? "inv" : "seq" ) << ","
            << c_.numcells() << "," << c_.sim_steps() << "," << c_.rep_steps() << "," << std::fixed
            << g_mbw_ << "," << a_mbw_ << "," << max_.mbw_ << "," << max_.size_ << "," << max_.rank_
            << "," << min_.mbw_ << "," << min_.size_ << "," << min_.rank_ << std::endl;
}

}
