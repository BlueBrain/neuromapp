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

#include "replib/backends/basic.h"
#include "replib/backends/mpiio.h"
#ifdef RL_HDF5
#include "replib/backends/hdf5p.h"
#endif
#ifdef RL_ADIOS
#include "replib/backends/adios.h"
#endif
#include "replib/utils/tools.h"
#include "replib/utils/config.h"
#include "replib/utils/statistics.h"


#include "utils/mpi/timer.h"
#include "utils/mpi/error.h"

namespace replib {

class benchmark {

    private:
        /** config structure with the benchmark parameters*/
        replib::config c_;
        replib::Writer * writer_;

    public:
        /** \fun benchmark(int argc, char* argv[])
        \brief create the benchmark and initialize it according to the
        given parameters
         */
        benchmark(int argc, char* const argv[]) : c_(argc, argv), writer_(NULL) {
            if (c_.backend() == "mpiio") {
                // MPI I/O backend
                writer_ = reinterpret_cast<replib::Writer*>(new replib::MPIIOWriter());
            } else if (c_.backend() == "h5p") {
                // Parallel HDF5
#ifdef RL_HDF5
                writer_ = reinterpret_cast<replib::Writer*>(new replib::H5PWriter());
#else
                std::cout << "Error: asked for parallel HDF5 backend, but HDF5 library was not found." << std::endl;
#endif
            } else if (c_.backend() == "adios") {
                // To complete
#ifdef RL_ADIOS
                // TODO: Create object for ADIOS backend
#else
                std::cout << "Error: asked for ADIOS backend, but ADIOS library was not found." << std::endl;
#endif
            } else {
                std::cout << "Error: unrecognized backend: '" << c_.backend() << "'. Supported backeds are: mpiio and adios" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 910);
            }

            // Initialize writer
            writer_->init(c_);
        }

        ~benchmark() {
            delete(writer_);
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

        /** \fun get_writer() const
        \brief return the writer backend object, read-only */
        replib::Writer const & get_writer() const {
            return * writer_;
        }

        /** \fun get_writer()
        \brief return the writer backend object */
        replib::Writer & get_writer() {
            return * writer_;
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
            writer_->init(c_);

            // Each process can have a different amount of data to write, ask the writer what is
            // the exact size for each process
            unsigned int sizeToWrite = writer_->total_bytes();

            // Double buffering
            float * buffer1 = (float *) malloc(sizeToWrite);
            float * buffer2 = (float *) malloc(sizeToWrite);

            memset(buffer1, 0, sizeToWrite);
            memset(buffer2, 0, sizeToWrite);

            float * bufferToFill = &buffer1[0];
            float * bufferToWrite = &buffer2[0];

            // c_str returns a 'const char *', but we need 'char *'
            char * report = strdup(c_.output_report().c_str());

            //Open the file
            writer_->open(report);

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

                // t_io only counts time spent in MPI_File_write_all to compute I/O statistics
                writer_->write(t_io, bufferToWrite, compCount);

                //std::stringstream ss2;
                //ss2 << "[" << c_.id() << "] [it" << i << "] size = " << compCount * sizeof(float)
                //<< " bytes ; time = " << t_io.time() << " seconds" << std::endl;
                //std::cout << mapp::mpi_filter_all() << ss2.str() << mapp::mpi_filter_master();

                welapsed.push_back(t_io.time());

                //std::cout << "[" << mpiRank << "] --------------------- Data written ---------------------" << std::endl;

                // Uncomment to ignore time spent in MPI write and comment corresponding line above
                //t_comp.tic();

                nwrites++;
            }
            MPI_Barrier(MPI_COMM_WORLD);

            writer_->close();

            writer_->finalize();

            free(buffer1);
            free(buffer2);

            MPI_Barrier(MPI_COMM_WORLD);
            if (c_.check()) {
                int passed = 0;
                if (c_.id() == 0) {
                    if (c_.backend() == "mpiio") {
                        c_.passed() = check_report(report, nwrites, c_.elems_per_step(), c_.procs(), units, decimals);
                        passed = c_.passed() ? 1 : 0;
                    } else {
                        std::cout << "WARNING: Test check only available for MPI I/O backend" << std::endl;
                        passed = 1;
                    }
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
            writer_->init(c_);

            unsigned int sizeToWrite = writer_->total_bytes();

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

            // Constants to calculate values of output buffer
            // Value = RANK_ID * units + reporting_step "." index_in_buffer
            // E.g.: Value = 23045.0117 --> Value written by rank 23, at the 45th
            // reporting iteration. This value corresponds to buffer1[117]
            // NOTE: In the IOR mode, reporting_step is always 0
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
            size_t bufElems = bufSize / sizeof(float);
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

            // c_str returns a 'const char *', but we need 'char *'
            char * report = strdup(c_.output_report().c_str());

            // Synchronize ranks before starting timer
            MPI_Barrier(MPI_COMM_WORLD);

            //Open the file
            writer_->open(t_io, c_.output_report());
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

                writer_->write(&buf[bufIdx], compCount);
                bufIdx += compCount;
                if (bufIdx >= bufElems) bufIdx = 0;

                //std::cout << "[" << mpiRank << "] --------------------- Data written ---------------------" << std::endl;

                nwrites++;
            }
            t_io.toc();
            welapsed.push_back(t_io.time());

            writer_->close(t_io);
            welapsed.push_back(t_io.time());

            writer_->finalize();

            //std::stringstream ss2;
            //ss2 << "[" << c_.id() << "] total size = " << compCount * sizeof(float) * nwrites << " bytes ; "
            //<< "MPI_File_open() time = " << welapsed[0] << " seconds" 
            //<< "MPI_File_write() time = " << welapsed[1] << " seconds" 
            //<< "MPI_File_close() time = " << welapsed[2] << " seconds" << std::endl;
            //std::cout << mapp::mpi_filter_all() << ss2.str() << mapp::mpi_filter_master();

            if (buf != buffer1) free(buf);
            free(buffer1);

            MPI_Barrier(MPI_COMM_WORLD);
            if (c_.check()) {
                int passed = 0;
                if (c_.id() == 0) {
                    if (c_.backend() == "mpiio") {
                        c_.passed() = check_report(report, nwrites, c_.elems_per_step(), c_.procs(), units, decimals);
                        passed = c_.passed() ? 1 : 0;
                    } else {
                        std::cout << "WARNING: Test check only available for MPI I/O backend" << std::endl;
                        passed = 1;
                    }
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
