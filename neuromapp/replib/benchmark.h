/*
 * Neuromapp - benchmark.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
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
 * GNU General Public License for more details. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#ifndef MAPP_BENCHMARK_H
#define MAPP_BENCHMARK_H

#include <vector>
#include <mpi.h>
#include <unistd.h>
#include <string.h>


#include "replib/utils/tools.h"
#include "replib/utils/config.h"
#include "replib/utils/statistics.h"
#include "replib/mpiio_dist/rnd1b.h"
#include "replib/mpiio_dist/file1b.h"
#include "replib/mpiio_dist/fileNb.h"

#include "utils/mpi/timer.h"

class benchmark {
public:
    /** \fun benchmark(int argc, char* argv[])
        \brief create the benchmark and initialize it according to the
        given parameters
     */
    benchmark(int argc, char* argv[]) : c_(argc, argv), f_(NULL) {
        init();
    }

    /** \fun benchmark()
        \brief create the benchmark and initialize it with default values
     */
    benchmark() : c_() {
        init();
    }

    ~benchmark() {
        delete(f_);
    }

    /** \fun get_config() const
        \brief return the configuration */
    replib::config const & get_config() const {
        return c_;
    }

    /** \fun get_config()
        \brief return the configuration */
    replib::config & get_config() {
        return c_;
    }

    /** \fun get_fileview() const
        \brief return the fileview */
    replib::fileview const & get_fileview() const {
        return *f_;
    }

    /** \fun get_fileview()
        \brief return the fileview */
    replib::fileview & get_fileview() {
        return *f_;
    }

private:
    /** config structure with the benchmark parameters*/
    replib::config c_;
    replib::fileview * f_;

    /** \fun init()
        \brief initialize the object */
    void init() {
        if (c_.write() == "file1b") {
            f_ = file1b(c_);
        } else if (c_.write() == "fileNb") {
            f_ = fileNb(c_);
        } else { // "rnd1b"
            f_ = rnd1b(c_);
        }
    }
};

/** \fun replib::statistics run_benchmark (benchmark & b)
    \brief Run the benchmark and return the object with the
    recorded statistics
 */
replib::statistics run_benchmark (benchmark & b) {

    // Double buffering
    unsigned int sizeToWrite = b.get_fileview().total_bytes();

    // Data is written to file every 10 reporting steps
    int reportToDisk = 10;
    sizeToWrite *= reportToDisk;

    float * buffer1 = (float *) malloc(sizeToWrite);
    float * buffer2 = (float *) malloc(sizeToWrite);

    memset(buffer1, 0, sizeToWrite);
    memset(buffer2, 0, sizeToWrite);

    float * bufferToFill = &buffer1[0];
    float * bufferToWrite = &buffer2[0];

    //Open the file
    MPI_File fh;
    char * report = strdup(b.get_config().output_report().c_str());
    int error = MPI_File_open(MPI_COMM_WORLD, report, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << b.get_config().id() << "] Error opening file" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 911);
    }

    b.get_fileview().set_fileview(&fh);

    // ReportingLib writes data to disk every 10 reporting steps
    int stepsToWrite = reportToDisk;
    int nwrites = 0;
    int compCount = sizeToWrite / sizeof(float); // Represents the number of compartments
    double compTimeUs = b.get_config().sim_time_ms() * 1000;
    double start, end, elapsed;

    std::vector<double> welapsed;
    welapsed.reserve(b.get_config().rep_steps());

    mapp::timer t;
    int steps = b.get_config().rep_steps() * reportToDisk;

    start = MPI_Wtime();
    for( int i = 0; i < steps; i++ ) {
        stepsToWrite--;

        // Rewrite vector
        for (unsigned int j = i%10; j < compCount; j += 10) {
            bufferToFill[j] = (float) b.get_config().id() * 1000.0 + (float) nwrites
                    + (float) ( ((float) ((j%1000) + 1.0)) / 1000.0);
        }

        if (stepsToWrite == 1) {
            end = MPI_Wtime();
            elapsed = (end - start) * 1e6; // us

            // The I/O write will happen in the next iteration, sleep to simulate
            // processing time
            int sleep = compTimeUs - elapsed;
            usleep( (sleep > 0) ? sleep : 0 );
        }

        //std::stringstream ss1;
        //ss1 << "[" << b.get_config().id() << "] [it" << i << "] Vector: ";
        //for (int s = 0; s < compCount; s++) {
        //    ss1 << std::fixed << std::setprecision(4) << bufferToFill[s] << " ";
        //}
        //ss1 << std::endl;
        //std::cout << mapp::mpi_filter_all() << ss1.str();

        if (stepsToWrite == 0) {
            // Account time spent in MPI write
            start = MPI_Wtime();

            // Swap buffers
            float * tmp = bufferToFill;
            bufferToFill = bufferToWrite;
            bufferToWrite = tmp;

            //std::cout << "[" << 0 << "] --------------------- Writing out data ---------------------" << std::endl;

            MPI_Status status;
            t.tic();
            error = MPI_File_write_all(fh, bufferToWrite, compCount, MPI_FLOAT, &status);
            t.toc();

            welapsed.push_back(t.time());

            if (error != MPI_SUCCESS) {
                std::cout << "[" << b.get_config().id() << "] Error writing file" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 915);
            }

            //std::cout << "[" << mpiRank << "] --------------------- Data written ---------------------" << std::endl;

            // Ignore time spent in MPI write
            //start = MPI_Wtime();

            stepsToWrite = 10;
            nwrites++;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    error = MPI_File_close(&fh);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << b.get_config().id() << "] Error closing file" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 912);
    }

    free(buffer1);
    free(buffer2);

    MPI_Barrier(MPI_COMM_WORLD);
    if (b.get_config().check() && b.get_config().id() == 0) {
        replib::check_report(report, nwrites, b.get_config().elems_per_step() * reportToDisk, b.get_config().procs());
    }

    replib::statistics s(b.get_config(), b.get_fileview().total_bytes(), welapsed);

    return s;
}

#endif
