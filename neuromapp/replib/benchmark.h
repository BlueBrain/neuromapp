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
#include <cstring>
#include <iomanip>


#include "replib/utils/tools.h"
#include "replib/utils/config.h"
#include "replib/utils/statistics.h"
#include "replib/mpiio_dist/rnd1b.h"
#include "replib/mpiio_dist/file1b.h"
#include "replib/mpiio_dist/fileNb.h"

#include "utils/mpi/timer.h"

namespace replib {

class benchmark {

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

    public:
        /** \fun benchmark(int argc, char* argv[])
        \brief create the benchmark and initialize it according to the
        given parameters
         */
        benchmark(int argc, char* const argv[]) : c_(argc, argv), f_(NULL) {
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

        /** \fun success()
        \brief return whether the execution of this benchmark succeeded or failed */
        bool success() const {
            return c_.passed();
        }

        /** \fun replib::statistics run_benchmark ()
        \brief Run the benchmark and return the object with the
        recorded statistics */
        replib::statistics run_benchmark ()
        {
            if (c_.sim_time_ms() == 0) {
                return run_ior_benchmark();
            } else {
                return run_replib_benchmark();
            }
        }

        /** \fun replib::statistics run_replib_benchmark ()
        \brief Run the benchmark as ReportingLib does and return the object with the
        recorded statistics */
        replib::statistics run_replib_benchmark ()
        {
            // Double buffering
            unsigned int sizeToWrite = f_->total_bytes();

            float * buffer1 = (float *) malloc(sizeToWrite);
            float * buffer2 = (float *) malloc(sizeToWrite);

            memset(buffer1, 0, sizeToWrite);
            memset(buffer2, 0, sizeToWrite);

            float * bufferToFill = &buffer1[0];
            float * bufferToWrite = &buffer2[0];

            // Use MPI_Info to disable collective buffers if using IME backend
            MPI_Info info;
            MPI_Info_create(&info);
            std::string ime("ime:/");
            if (c_.output_report().compare(0, ime.length(), ime) == 0) {
                MPI_Info_set (info, "romio_ds_write", "disable");
                MPI_Info_set (info, "romio_cb_write", "disable");
            }

            //Open the file
            MPI_File fh;
            char * report = strdup(c_.output_report().c_str());
            int error = MPI_File_open(MPI_COMM_WORLD, report, MPI_MODE_WRONLY | MPI_MODE_CREATE, info, &fh);

            if (error != MPI_SUCCESS) {
                std::cout << "[" << c_.id() << "] Error opening file: " << report << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 911);
            }

            f_->set_fileview(&fh);

            int nwrites = 0;
            int compCount = sizeToWrite / sizeof(float); // Represents the number of compartments
            double compTimeUs = c_.sim_time_ms() * 1000;

            std::vector<double> welapsed;
            welapsed.reserve(c_.rep_steps());

            // Timer used to compute the time spent inside MPI I/O collective only
            mapp::timer t_io;

            // Timer used to simulate the computational phase
            mapp::timer t_comp;

            // Number of reporting steps
            int rep_steps = c_.rep_steps();

            // Number of simulation steps for each reporting step
            int sim_steps = c_.sim_steps();

            // Constants to calculate values of output buffer
            // Value = RANK_ID * units + reporting_step "." index_in_buffer
            // E.g.: Value = 23045.0117 --> Value written by rank 23, at the 45th
            // reporting iteration. This value corresponds to bufferToFill[117]
            float units = 10.0;
            float nsteps = (float) rep_steps;
            while ((int) nsteps > 0) {
                nsteps /= 10.0;
                units *= 10.0;
            }
            int decimals = 10.0;
            float npos = (float) compCount;
            while ((int) npos > 0) {
                npos /= 10.0;
                decimals *= 10.0;
            }

            t_comp.tic();
            for (int i = 0; i < rep_steps; i++) {
                // Simulate computational phase
                for (int j = 0; j < sim_steps; j++) {
                    // Rewrite vector to avoid caching everything
                    for (int k = j%sim_steps; k < compCount; k += sim_steps) {
                        bufferToFill[k] = (float) c_.id() * units + (float) nwrites
                                + (float) (((float) ((k%decimals) + 1.0)) / (float) decimals);
                    }
                }
                t_comp.toc();

                // Complete computational phase with sleep
                double elapsed = t_comp.time() * 1e6; // elapsed in us
                int sleep = compTimeUs - elapsed;
                usleep( (sleep > 0) ? sleep : 0 );

                //std::stringstream ss1;
                //ss1 << "[" << c_.id() << "] [it" << i << "] Vector: ";
                //for (int s = 0; s < compCount; s++) {
                //    ss1 << std::fixed << std::setprecision(4) << bufferToFill[s] << " ";
                //}
                //ss1 << std::endl;
                //std::cout << mapp::mpi_filter_all() << ss1.str() << mapp::mpi_filter_master();

                // Uncomment to account time spent by MPI write in t_comp as well and comment corresponding line below
                t_comp.tic();

                // Swap buffers
                float * tmp = bufferToFill;
                bufferToFill = bufferToWrite;
                bufferToWrite = tmp;

                //std::cout << "[" << 0 << "] --------------------- Writing out data ---------------------" << std::endl;

                MPI_Status status;
                // t_io only counts time spent in MPI_File_write_all to compute I/O statistics
                t_io.tic();
                error = MPI_File_write_all(fh, bufferToWrite, compCount, MPI_FLOAT, &status);
                t_io.toc();

                //std::stringstream ss2;
                //ss2 << "[" << c_.id() << "] [it" << i << "] size = " << compCount * sizeof(float)
                //<< " bytes ; time = " << t_io.time() << " seconds" << std::endl;
                //std::cout << mapp::mpi_filter_all() << ss2.str() << mapp::mpi_filter_master();

                welapsed.push_back(t_io.time());

                if (error != MPI_SUCCESS) {
                    std::cout << "[" << c_.id() << "] Error writing file" << std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 915);
                }

                //std::cout << "[" << mpiRank << "] --------------------- Data written ---------------------" << std::endl;

                // Uncomment to ignore time spent in MPI write and comment corresponding line above
                //t_comp.tic();

                nwrites++;
            }
            MPI_Barrier(MPI_COMM_WORLD);

            error = MPI_File_close(&fh);
            if (error != MPI_SUCCESS) {
                std::cout << "[" << c_.id() << "] Error closing file" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 912);
            }

            free(buffer1);
            free(buffer2);

            MPI_Barrier(MPI_COMM_WORLD);
            if (c_.check()) {
                int passed = 0;
                if (c_.id() == 0) {
                    c_.passed() = check_report(report, nwrites, c_.elems_per_step(), c_.procs(), units, decimals);
                    passed = c_.passed() ? 1 : 0;
                }
                MPI_Bcast(&passed, 1, MPI_INT, 0, MPI_COMM_WORLD);
                c_.passed() = (passed == 1) ? true : false;
            }

            replib::statistics s(c_, sizeToWrite, welapsed);

            return s;
        }

        /** \fun replib::statistics run_ior_benchmark ()
        \brief Run the benchmark as IOR does and return the object with the
        recorded statistics */
        replib::statistics run_ior_benchmark ()
        {
            unsigned int sizeToWrite = f_->total_bytes();

            float * buffer1 = (float *) malloc(sizeToWrite);

            memset(buffer1, 0, sizeToWrite);

            int nwrites = 0;
            int compCount = sizeToWrite / sizeof(float); // Represents the number of compartments

            // welapsed[0] --> time for opening the file
            // welapsed[1] --> time for writing
            // welapsed[2] --> time for closing the file
            std::vector<double> welapsed;
            welapsed.reserve(3);

            // Timer used to compute the time spent inside each MPI I/O phase (open, write, close)
            mapp::timer t_io;

            // Number of reporting steps
            int rep_steps = c_.rep_steps();

            // Number of simulation steps for each reporting step
            int sim_steps = c_.sim_steps();

            // Constants to calculate values of output buffer
            // Value = RANK_ID * units + reporting_step "." index_in_buffer
            // E.g.: Value = 23045.0117 --> Value written by rank 23, at the 45th
            // reporting iteration. This value corresponds to buffer1[117]
            float units = 10.0;
            float nsteps = (float) rep_steps;
            while ((int) nsteps > 0) {
                nsteps /= 10.0;
                units *= 10.0;
            }
            int decimals = 10.0;
            float npos = (float) compCount;
            while ((int) npos > 0) {
                npos /= 10.0;
                decimals *= 10.0;
            }

            // Initialize buffer1
            for (int k = 0; k < compCount; k++) {
                buffer1[k] = (float) c_.id() * units + (float) nwrites
                        + (float) (((float) ((k%decimals) + 1.0)) / (float) decimals);
            }

            // Create a huge buffer to avoid caching effects at writing
            // First attempt: 1 GB
            size_t bufSize = 1 * 1024 * 1024 * 1024;
            float * buf = (float *) malloc(bufSize);

            // If malloc fails (returns 0), try smaller values
            // Minimum size to allocate is the size needed for 1 iteration
            while (buf == 0) {
                bufSize /= 2;
                if (bufSize < sizeToWrite) {
                    // Could not allocate the buffer, use the original buffer1
                    bufSize = sizeToWrite;
                    buf = buffer1;
                    break;
                }
                buf = (float *) malloc(bufSize);
            }

            // Number of times 'buffer1' vector fits in 'buf'
            unsigned int times = bufSize / sizeToWrite;
            // Set _bufferSize accordingly and ignore the remaining part of the buffer
            bufSize = times * sizeToWrite;
            // Index _buffer as a char *
            size_t offset = 0;
            // Copy 'buffer1' into 'buf'
            if (buf != buffer1) {
                for (unsigned int t = 0; t < times; t++) {
                    std::memcpy(&buf[offset], buffer1, sizeToWrite);
                    offset += sizeToWrite / sizeof(float);
                }
            }

            // Keep the index in buf to control when we reach the end and start over
            unsigned int bufIdx = 0;

            // Use MPI_Info to disable collective buffers if using IME backend
            MPI_Info info;
            MPI_Info_create(&info);
            std::string ime("ime:/");
            if (c_.output_report().compare(0, ime.length(), ime) == 0) {
                MPI_Info_set (info, "romio_ds_write", "disable");
                MPI_Info_set (info, "romio_cb_write", "disable");
            }

            // Synchronize ranks before starting timer
            MPI_Barrier(MPI_COMM_WORLD);

            //Open the file
            MPI_File fh;
            char * report = strdup(c_.output_report().c_str());
            t_io.tic();
            int error = MPI_File_open(MPI_COMM_WORLD, report, MPI_MODE_WRONLY | MPI_MODE_CREATE, info, &fh);
            if (error != MPI_SUCCESS) {
                std::cout << "[" << c_.id() << "] Error opening file" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 911);
            }

            f_->set_fileview(&fh);
            t_io.toc();
            welapsed.push_back(t_io.time());

            t_io.tic();
            for (int i = 0; i < rep_steps; i++) {
                //std::stringstream ss1;
                //ss1 << "[" << c_.id() << "] [it" << i << "] Vector: ";
                //for (int s = 0; s < compCount; s++) {
                //    ss1 << std::fixed << std::setprecision(4) << buf[s] << " ";
                //}
                //ss1 << std::endl;
                //std::cout << mapp::mpi_filter_all() << ss1.str() << mapp::mpi_filter_master();

                //std::cout << "[" << 0 << "] --------------------- Writing out data ---------------------" << std::endl;

                MPI_Status status;
                error = MPI_File_write_all(fh, &buf[bufIdx], compCount, MPI_FLOAT, &status);
                bufIdx += compCount;
                if (bufIdx >= bufSize) bufIdx = 0;

                if (error != MPI_SUCCESS) {
                    std::cout << "[" << c_.id() << "] Error writing file" << std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 915);
                }

                //std::cout << "[" << mpiRank << "] --------------------- Data written ---------------------" << std::endl;

                nwrites++;
            }
            t_io.toc();
            welapsed.push_back(t_io.time());

            //std::stringstream ss2;
            //ss2 << "[" << c_.id() << "] total size = " << compCount * sizeof(float) * nwrites
            //<< " bytes ; write only time = " << t_io.time() << " seconds" << std::endl;
            //std::cout << mapp::mpi_filter_all() << ss2.str() << mapp::mpi_filter_master();

            t_io.tic();
            error = MPI_File_close(&fh);
            if (error != MPI_SUCCESS) {
                std::cout << "[" << c_.id() << "] Error closing file" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 912);
            }
            t_io.toc();
            welapsed.push_back(t_io.time());

            if (buf != buffer1) free(buf);
            free(buffer1);

            MPI_Barrier(MPI_COMM_WORLD);
            if (c_.check()) {
                int passed = 0;
                if (c_.id() == 0) {
                    c_.passed() = check_report(report, nwrites, c_.elems_per_step(), c_.procs(), units, decimals);
                    passed = c_.passed() ? 1 : 0;
                }
                MPI_Bcast(&passed, 1, MPI_INT, 0, MPI_COMM_WORLD);
                c_.passed() = (passed == 1) ? true : false;
            }

            replib::statistics s(c_, sizeToWrite, welapsed);

            return s;
        }
};

} // end namespace

#endif
