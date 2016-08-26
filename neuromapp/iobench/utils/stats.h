/*
 * Neuromapp - stats.h, Copyright (c), 2015,
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
 * @file neuromapp/iobench/utils/stats.h
 * \brief basic shell for statistics
 */

#ifndef MAP_IOBENCH_STATS_H
#define MAP_IOBENCH_STATS_H

#include <vector>
#include <numeric>
#include <cmath>

#ifdef IO_MPI
#include <mpi.h>
#endif

namespace iobench {

class stats {

    private:
        std::vector<double> bw_;
        std::vector<double> iops_;

        double              mb_;
        unsigned int        ops_;
        double              time_;
        double              avgBW_;
        double              stddevBW_;
        double              stderrBW_;
        double              avgIOPS_;
        double              stddevIOPS_;
        double              stderrIOPS_;

    public:

        stats() : bw_(), iops_(), mb_(0.0), ops_(0), time_(0.0), avgBW_(0.0), stddevBW_(0.0),
        stderrBW_(0.0), avgIOPS_(0.0), stddevIOPS_(0.0), stderrIOPS_(0.0) {}


        /**
        \brief record a new time (in seconds) to compute BW and IOPS value
        */
        void record (double time) {
            time_ += time;
            bw_.push_back(mb_ / time);
            iops_.push_back((double) ops_ / time);
        }

        /**
        \brief return the amount of bytes (in MB), read only
        */
        inline double mb() const {
            return mb_;
        }

        /**
        \brief return the amount of operations, read only
        */
        inline unsigned int ops() const {
            return ops_;
        }

        /**
        \brief return the average bandwidth (in MB/s), read only
        */
        inline double avgBW() const {
            return avgBW_;
        }

        /**
        \brief return the standard deviation of the bandwidth, read only
        */
        inline double stddevBW() const {
            return stddevBW_;
        }

        /**
        \brief return the standard error of the bandwidth, read only
        */
        inline double stderrBW() const {
            return stderrBW_;
        }

        /**
        \brief return the average IOPS (in I/O op/s), read only
        */
        inline double avgIOPS() const {
            return avgIOPS_;
        }

        /**
        \brief return the standard deviation of the IOPS, read only
        */
        inline double stddevIOPS() const {
            return stddevIOPS_;
        }

        /**
        \brief return the standard error of the IOPS, read only
        */
        inline double stderrIOPS() const {
            return stderrIOPS_;
        }


        /**
        \brief return the amount of bytes (in MB), write only
        */
        inline double &mb() {
            return mb_;
        }

        /**
        \brief return the amount of operations, write only
        */
        inline unsigned int &ops() {
            return ops_;
        }

        /** \fun void compute_stats()
        \brief Compute the statistics for BW and IOPS
         */
        void compute_stats(int mpi_rank, int mpi_size) {
            compute_statistics(bw_, avgBW_, stddevBW_, stderrBW_);
            compute_statistics(iops_, avgIOPS_, stddevIOPS_, stderrIOPS_);
#ifdef IO_MPI
            MPI_Barrier(MPI_COMM_WORLD);

            if (mpi_size > 1) {
                if (mpi_rank == 0) {
                    std::vector<double> bws, iopss;
                    bws.reserve(mpi_size);
                    iopss.reserve(mpi_size);

                    bws.push_back(avgBW_);
                    iopss.push_back(avgIOPS_);

                    for (int i = 1; i < mpi_size; i++) {
                        bws.push_back(0.0);
                        iopss.push_back(0.0);

                        MPI_Status status;
                        MPI_Recv(&bws.back(), 1, MPI_DOUBLE, i, i * 100 + 0, MPI_COMM_WORLD, &status);
                        MPI_Recv(&iopss.back(), 1, MPI_DOUBLE, i, i * 100 + 1, MPI_COMM_WORLD, &status);

                        double time;
                        MPI_Recv(&time, 1, MPI_DOUBLE, i, i * 100 + 2, MPI_COMM_WORLD, &status);
                        time_ += time;
                    }

                    // Statistics
                    mb_ = mb_ * mpi_size;
                    ops_ = ops_ * mpi_size;

                    compute_statistics(bws, avgBW_, stddevBW_, stderrBW_);
                    compute_statistics(iopss, avgIOPS_, stddevIOPS_, stderrIOPS_);
               } else {
                    MPI_Send(&avgBW_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 0, MPI_COMM_WORLD);
                    MPI_Send(&avgIOPS_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 1, MPI_COMM_WORLD);
                    MPI_Send(&time_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 2, MPI_COMM_WORLD);
               }
            }

#endif
        }

        /** \brief the print function */
        void print(std::ostream& out) const{
            out << "  Total time (s): " << time_ << " \n"
                    << "  Total data (MB): " << mb_ << " \n"
                    << "  Total IOPS (I/O op/s): " << ops_ << " \n"
                    << "  Avg BW (MB/s): " << avgBW_ << " \n"
                    << "    BW std dev: " << stddevBW_ << " \n"
                    << "    BW std err: " << stderrBW_ << " \n"
                    << "  Avg IOPS (I/O op/s): " << avgIOPS_ << " \n"
                    << "    IOPS std dev: " << stddevIOPS_ << " \n"
                    << "    IOPS std err: " << stderrIOPS_ << " \n";
        }

    private:
        /** \fun void compute_statistics(const std::vector<double> &values, double & avg, double & stddev, double & stderr)
        \brief Compute the statistics: given a vector of double values, compute the average, standard deviation and standard error
         */
        void compute_statistics(const std::vector<double> &values, double & avg, double & stddev, double & stderr)
        {
            // Average
            int n = values.size();
            avg = std::accumulate(values.begin(), values.end(), 0.0) / n;

            // Std deviation
            stddev = 0.0;
            for (int i = 0; i < n; i++) {
                stddev += (values[i] - avg) * (values[i] - avg);
            }
            stddev = sqrt(1.0 / n * stddev);

            // Std error
            stderr = stddev / sqrt(n);
        }
};

/** \brief basic overload the ostream operator to print the stats */
inline std::ostream &operator<<(std::ostream &out, stats const& s){
     s.print(out);
     return out;
}

} //end namespace

#endif // MAP_IOBENCH_STATS_H
