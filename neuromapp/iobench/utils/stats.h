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
#include <iostream>

#ifdef IO_MPI
#include <mpi.h>
#endif

namespace iobench {

struct Stats_descriptive_basic{
    Stats_descriptive_basic(): avg_(0.0), stddev_(0.0), stderr_(0.0)
    {}
    double avg_;
    double stddev_;
    double stderr_;
};

/** \brief basic overload the ostream operator to print Stats_descriptive_basic */
inline std::ostream &operator<<(std::ostream &out, Stats_descriptive_basic const& d){
     out << "avg:" << d.avg_ << " stddev:" << d.stddev_ << " stderr:" << d.stderr_;
     return out;
};

class stats {
    private:

        std::vector <double> rec_time_;
        std::vector <double> rec_mb_;
        std::vector <double> rec_ops_;

        double              total_time_;
        double              total_mb_;
        double              total_ops_;

        double              avgMB_per_cycle_;
        double              avgOPS_per_cycle_;

        Stats_descriptive_basic        OPS_;
        Stats_descriptive_basic        MB_;

        Stats_descriptive_basic        BW_;
        Stats_descriptive_basic        IOPS_;

    public:
        /**
        \brief record a new time (in seconds) to compute BW and IOPS value
        */
        void record (double time, double mb, unsigned int ops) {
            rec_time_.push_back( time );
            rec_mb_.push_back( mb );
            rec_ops_.push_back( ops );
        }

        /**
        \brief return total measured time (seconds), read only
        */
        inline double total_time() const {
            return total_time_;
        }

        /**
        \brief return total data size (MB), read only
        */
        inline double total_mb() const {
            return total_mb_;
        }

        /**
        \brief return total number of operations (I/O ops), read only
        */
        inline double total_ops() const {
            return total_ops_;
        }

        /**
        \brief return the average number of read operations over cycle, read only
        */
        inline double avgOPS_per_cycle() const {
            return avgOPS_per_cycle_;
        }

        /**
        \brief return the average data size (MB) over cycle, read only
        */
        inline double avgMB_per_cycle() const {
            return avgMB_per_cycle_;
        }

        /**
        \brief return the average number of read operations, read only
        */
        inline double avgOPS() const {
            return OPS_.avg_;
        }

        /**
        \brief return the standard deviation of number of read operations, read only
        */
        inline double stddevOPS() const {
            return OPS_.stddev_;
        }

        /**
        \brief return the standard error of number of read operations, read only
        */
        inline double stderrOPS() const {
            return OPS_.stderr_;
        }

        /**
        \brief return the average data size (MB), read only
        */
        inline double avgMB() const {
            return MB_.avg_;
        }

        /**
        \brief return the standard deviation of the data size, read only
        */
        inline double stddevMB() const {
            return MB_.stddev_;
        }

        /**
        \brief return the standard error of the data size, read only
        */
        inline double stderrMB() const {
            return MB_.stderr_;
        }

        /**
        \brief return the average bandwidth (in MB/s), read only
        */
        inline double avgBW() const {
            return BW_.avg_;
        }

        /**
        \brief return the standard deviation of the bandwidth, read only
        */
        inline double stddevBW() const {
            return BW_.stddev_;
        }

        /**
        \brief return the standard error of the bandwidth, read only
        */
        inline double stderrBW() const {
            return BW_.stderr_;
        }

        /**
        \brief return the average IOPS (in I/O op/s), read only
        */
        inline double avgIOPS() const {
            return IOPS_.avg_;
        }

        /**
        \brief return the standard deviation of the IOPS, read only
        */
        inline double stddevIOPS() const {
            return IOPS_.stddev_;
        }

        /**
        \brief return the standard error of the IOPS, read only
        */
        inline double stderrIOPS() const {
            return IOPS_.stderr_;
        }

        inline bool check_valid_record(const double& time, const double& mb, const unsigned int& ops) {
            return (mb>(1./1024./1024.) && time > 0. && ops > 0);
        }


        /** \fun void compute_stats()
        \brief Compute the statistics for BW and IOPS
         */
        void compute_stats(int mpi_rank, int mpi_size) {
            //remove not valid entries
            std::vector< double > val_rec_time;
            std::vector< double > val_rec_mb;
            std::vector< double > val_rec_ops;
            for (unsigned int i=0; i<rec_time_.size(); i++)
                if (check_valid_record(rec_time_[i], rec_mb_[i], rec_ops_[i])) {
                    val_rec_time.push_back(rec_time_[i]);
                    val_rec_mb.push_back(rec_mb_[i]);
                    val_rec_ops.push_back(rec_ops_[i]);
                }
            rec_time_.swap(val_rec_time);
            rec_mb_.swap(val_rec_mb);
            rec_ops_.swap(val_rec_ops);

            //continues with only valid entries
            int n_recs = rec_time_.size();
            std::vector< double > bw(n_recs);
            std::vector< double > iops(n_recs);
            for (int i=0; i<n_recs; i++){
                bw[i] = rec_mb_[i] / rec_time_[i];
                iops[i] = rec_ops_[i] / rec_time_[i];
            }

            double avg_recs_per_rank = n_recs;

            total_time_ = std::accumulate(rec_time_.begin(), rec_time_.end(), 0.0);
            total_mb_ = std::accumulate(rec_mb_.begin(), rec_mb_.end(), 0.0);
            total_ops_ = std::accumulate(rec_ops_.begin(), rec_ops_.end(), 0.0);

            summarize_statistics(rec_ops_, OPS_);
            summarize_statistics(rec_mb_, MB_);
            summarize_statistics(bw, BW_);
            summarize_statistics(iops, IOPS_);

#ifdef IO_MPI
            MPI_Barrier(MPI_COMM_WORLD);


            if (mpi_size > 1) {
                if (mpi_rank == 0) {
                    std::vector<double> mbs, opss, bws, iopss, recs_per_ranks;
                    mbs.reserve(mpi_size);
                    opss.reserve(mpi_size);
                    bws.reserve(mpi_size);
                    iopss.reserve(mpi_size);
                    recs_per_ranks.reserve(mpi_size);

                    mbs.push_back(MB_.avg_);
                    opss.push_back(OPS_.avg_);
                    bws.push_back(BW_.avg_);
                    iopss.push_back(IOPS_.avg_);
                    recs_per_ranks.push_back(avg_recs_per_rank);

                    for (int i = 1; i < mpi_size; i++) {
                        double rank_time, rank_mb, rank_ops;
                        MPI_Status status;
                        MPI_Recv(&rank_time, 1, MPI_DOUBLE, i, i * 100 + 0, MPI_COMM_WORLD, &status);
                        MPI_Recv(&rank_mb, 1, MPI_DOUBLE, i, i * 100 + 1, MPI_COMM_WORLD, &status);
                        MPI_Recv(&rank_ops, 1, MPI_DOUBLE, i, i * 100 + 2, MPI_COMM_WORLD, &status);

                        if (check_valid_record(rank_time, rank_mb, rank_ops)) {
                            total_time_ += rank_time;
                            total_mb_ += rank_mb;
                            total_ops_ += rank_ops;

                            mbs.push_back(0.0);
                            opss.push_back(0.0);
                            bws.push_back(0.0);
                            iopss.push_back(0.0);
                            recs_per_ranks.push_back(0.0);

                            MPI_Recv(&mbs.back(), 1, MPI_DOUBLE, i, i * 100 + 3, MPI_COMM_WORLD, &status);
                            MPI_Recv(&opss.back(), 1, MPI_DOUBLE, i, i * 100 + 4, MPI_COMM_WORLD, &status);
                            MPI_Recv(&bws.back(), 1, MPI_DOUBLE, i, i * 100 + 5, MPI_COMM_WORLD, &status);
                            MPI_Recv(&iopss.back(), 1, MPI_DOUBLE, i, i * 100 + 6, MPI_COMM_WORLD, &status);
                            MPI_Recv(&recs_per_ranks.back(), 1, MPI_DOUBLE, i, i * 100 + 7, MPI_COMM_WORLD, &status);
                        }
                    }
                    Stats_descriptive_basic recs_per_rank_distri;
                    summarize_statistics(recs_per_ranks, recs_per_rank_distri);
                    avg_recs_per_rank = recs_per_rank_distri.avg_;

                    summarize_statistics(mbs, MB_);
                    summarize_statistics(opss, OPS_);
                    summarize_statistics(bws, BW_);
                    summarize_statistics(iopss, IOPS_);
                } else {
                    MPI_Send(&total_time_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 0, MPI_COMM_WORLD);
                    MPI_Send(&total_mb_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 1, MPI_COMM_WORLD);
                    MPI_Send(&total_ops_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 2, MPI_COMM_WORLD);

                    if (check_valid_record(total_time_, total_mb_, total_ops_)) {
                        MPI_Send(&MB_.avg_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 3, MPI_COMM_WORLD);
                        MPI_Send(&OPS_.avg_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 4, MPI_COMM_WORLD);
                        MPI_Send(&BW_.avg_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 5, MPI_COMM_WORLD);
                        MPI_Send(&IOPS_.avg_, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 6, MPI_COMM_WORLD);
                        MPI_Send(&avg_recs_per_rank, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 7, MPI_COMM_WORLD);
                    }
                }
            }

#endif

            avgMB_per_cycle_ = total_mb_ / avg_recs_per_rank;
            avgOPS_per_cycle_ = total_ops_ / avg_recs_per_rank;
        }

        /** \brief the print function */
        void print(std::ostream& out) const{
            out << "  Total time (s):\n\t" << total_time_ << " \n"
                    << "  Total size (MB):\n\t" << total_mb_ << " \n"
                    << "  Total OPS (I/O op):\n\t" << total_ops_ << " \n"
                    << "  Avg data size per cycle (MB/cycle):\n\t" << avgMB_per_cycle_ << " \n"
                    << "  Avg OPS per cycle (I/O op/rec):\n\t" << avgOPS_per_cycle_ << " \n"
                    << "  data size (MB):\n\t" << MB_ << " \n"
                    << "  OPS (I/O op):\n\t" << OPS_ << " \n"
                    << "  BW (MB/s):\n\t" << BW_ << " \n"
                    << "  IOPS (I/O op/s):\n\t" << IOPS_ << " \n";
        }

    private:
        /** \fun void compute_statistics(const std::vector<double> &values, double & avg, double & stddev, double & stderr)
        \brief Compute the statistics: given a vector of double values, compute the average, standard deviation and standard error
         */
        void summarize_statistics(const std::vector<double> &values, Stats_descriptive_basic & v)
        {
            // Average
            int n = values.size();
            v.avg_ = std::accumulate(values.begin(), values.end(), 0.0) / n;

            // Std deviation
            v.stddev_ = 0.0;
            for (int i = 0; i < n; i++) {
                v.stddev_ += (values[i] - v.avg_) * (values[i] - v.avg_);
            }
            v.stddev_ = sqrt(1.0 / n * v.stddev_);

            // Std error
            v.stderr_ = v.stddev_ / sqrt(n);
        }
};

/** \brief basic overload the ostream operator to print the stats */
inline std::ostream &operator<<(std::ostream &out, stats const& s){
     s.print(out);
     return out;
}

} //end namespace

#endif // MAP_IOBENCH_STATS_H
