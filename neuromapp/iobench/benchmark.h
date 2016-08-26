/*
 * Neuromapp - benchmark.hpp, Copyright (c), 2015,
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
 * GNU General Public License for more details..  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/iobench/benchmark.h
 * iobench Miniapp: benchmark class
 */

#ifndef MAPP_IOBENCH_BENCHMARK_
#define MAPP_IOBENCH_BENCHMARK_

#include <omp.h>
#include <map>

#include "iobench/backends/basic.h"

#include "iobench/utils/args.h"
#include "iobench/utils/stats.h"

namespace iobench {

class benchmark {

    private:
        args a_;
        stats wrst_;
        stats rdst_;

        std::vector<BaseKV *> db_;

        std::vector<char *> keys_;
        std::vector<char *> values_;
        std::vector<char *> reads_;

public:
    /** \fun benchmark(int argc, char* argv[])
        \brief create the benchmark and initialize it according to the
        given parameters
     */
    benchmark(int argc, char* argv[]) : a_(argc, argv), wrst_(), rdst_() {
        init(argc, argv);
        initDB();
    }

    ~benchmark() {
        BaseKV * last = NULL;
        for (std::vector<BaseKV *>::iterator it = db_.begin();
                it != db_.end(); it++) {
            BaseKV * p = *it;
            if (last != p) {
                // Only delete if the DB instance is different (!single_db)
                delete p;
                last = p;
            }
        }
        for (std::vector<char *>::iterator it = keys_.begin();
                it != keys_.end(); it++) {
            delete [] *it;
        }
        for (std::vector<char *>::iterator it = values_.begin();
                it != values_.end(); it++) {
            delete [] *it;
        }
        for (std::vector<char *>::iterator it = reads_.begin();
                it != reads_.end(); it++) {
            delete [] *it;
        }

        finalize();
    }

    /** \fun createData()
        \brief create the data used by the benchmark
     */
    void createData();

    /** \fun run()
        \brief run the benchmark
     */
    void run() {
        if (a_.write())
            write(wrst_);

        if (a_.read())
            read(rdst_);
    }

    /** \brief the print function */
    void print_stats(std::ostream& out) {
#ifdef IO_MPI
        if (a_.rank() == 0) {
#endif
        out << a_;
        if (a_.write()) {
            out << "WRITE stats:\n"
            << wrst_;
        }
        if (a_.read()) {
            out << "READ stats:\n"
            << rdst_;
        }

        double mb = (a_.write() ? wrst_.mb() : rdst_.mb());
        // CSV line for easier processing
        out << "IOMAPP," << a_.procs() << "," << a_.threads() << "," << a_.backend() << "," << mb << ","
                << wrst_.avgBW() << "," << wrst_.stddevBW() << "," << wrst_.stderrBW() << ","
                << wrst_.avgIOPS() << "," << wrst_.stddevIOPS() << "," << wrst_.stderrIOPS() << ","
                << rdst_.avgBW() << "," << rdst_.stddevBW() << "," << rdst_.stderrBW() << ","
                << rdst_.avgIOPS() << "," << rdst_.stddevIOPS() << "," << rdst_.stderrIOPS() << "\n";
#ifdef IO_MPI
        }
#endif
    }

private:
    /** \fun init(int argc, char* argv[])
        \brief if MPI enabled, initialize MPI
     */
    void init(int argc, char* argv[]);

    /** \fun finalize()
        \brief if MPI enabled, finalize MPI
     */
    void finalize();

    /** \fun initDB()
        \brief initialize DB structures
     */
    void initDB();

    /** \fun write()
        \brief run the write benchmark
     */
    void write(stats & stats);

    /** \fun read()
        \brief run the read benchmark
     */
    void read(stats & stats);


    /** \fun get_args()
        \brief return the args object of the benchmark
     */
    args & get_args() {
        return a_;
    }
};

}

#endif // MAPP_IOBENCH_BENCHMARK_
