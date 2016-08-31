/*
 * Neuromapp - benchmark.cpp, Copyright (c), 2015,
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
 * @file neuromapp/iobench/benchmark.cpp
 * iobench Miniapp: benchmark class implementation
 */

#include <sstream>
#include <sys/time.h>
#include <stdio.h>

#include "iobench/benchmark.h"
#include "iobench/backends/common.h"

// Get OMP header if available
#include "utils/omp/compatibility.h"

#ifdef IO_MPI
#include <mpi.h>
#include "utils/mpi/print.h"
#endif

using namespace iobench;

void iobench::benchmark::init(int argc, char* const argv[]) {
#ifdef IO_MPI
    // MPI_Init() called when creating args
    //MPI_Init(&argc, &argv);
    int mpi_size, mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    std::cout << mapp::mpi_filter_all(); // MPI print master only move to mpi_filter_master for master only

    std::stringstream ss;
    ss << "I'm rank " << mpi_rank << " in a world of " << mpi_size << std::endl;
    std::cout << ss.str();

    a_.procs() = mpi_size;
    a_.rank() = mpi_rank;
#endif
}

void iobench::benchmark::finalize() {
#ifdef IO_MPI
    MPI_Finalize();
#endif
}

void iobench::benchmark::initDB() {
    bool single_db = true;

    if (!single_db) {
        for (int i = 0; i < a_.threads(); i++) {
            db_.push_back(createDB(a_.backend()));
        }
    } else {
        BaseKV * base = createDB(a_.backend());
        for (int i = 0; i < a_.threads(); i++) {
            db_.push_back(base);
        }
    }

#ifdef IO_MPI
    db_[0]->initDB(a_.compress(), a_.threads(), a_.npairs(), a_.rank(), a_.procs());
#else
    if (!single_db) {
        for (int i = 0; i < db_.size(); i++) {
            db_[i]->initDB(a_.compress(), i, a_.threads());
        }
    } else {
        db_[0]->initDB(a_.compress(), a_.threads(), a_.npairs());
    }
#endif
}



void iobench::benchmark::createData() {
    // Setting up k/v data
    keys_.reserve(a_.threads() * a_.npairs());
    for (int i = 0; i < a_.threads() * a_.npairs(); i++)
        keys_.push_back(new char[a_.keysize()]);

    if (a_.write()) {
        values_.reserve(a_.threads() * a_.npairs());
        for (int i = 0; i < a_.threads() * a_.npairs(); i++)
            values_.push_back(new char[a_.valuesize()]);
    }

    if (a_.read()) {
        reads_.reserve(a_.threads() * a_.npairs());
        for (int i = 0; i < a_.threads() * a_.npairs(); i++)
            reads_.push_back(new char[a_.valuesize()]);
    }

    char chars[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                    '1','2','3','4','5','6','7','8','9','0','-','+','=',';',':',',','.','?','/','*','<','>','[',']','{','}'};

#pragma omp parallel default(shared) //shared(keys_, values_, reads_) not allowed since they are class members
    {
        int id = omp_get_thread_num();
        int start_idx = id * a_.npairs();
        for (int i = 0; i < a_.npairs(); i++) {
            // Set keys
            sprintf(keys_[start_idx], "%i_%i", id, i);

            if (a_.write()) {
                if (a_.backend() == "cassandra") {
                    // Haven't been able to figure out the proper way to store an array
                    // of floats into Cassandra...
                    // 'Blob' or 'bytes' may be the right way to go:
                    // http://stackoverflow.com/questions/26805458/datastax-c-c-driver-for-apache-cassandra-blob-conversion-gcrypt-crypto-key
                    std::vector<char> voltages;
                    for (unsigned int j = 0; j < a_.valuesize(); j++) {
                        int idx = std::rand() % sizeof(chars); // sizeof(chars) should be 52
                        voltages.push_back( chars[idx] );
                    }
                    std::memcpy(values_[start_idx], &voltages[0], a_.valuesize());
                    values_[start_idx][a_.valuesize()-1] = '\0';

                } else {
                    // Set values: use realistic voltage values
                    int numVoltages = (a_.valuesize() / sizeof(float)) + 1;
                    std::vector<float> voltages;
                    for (int j = 0; j < numVoltages; j++) {
                        // Decide the voltage sign: -1 or 1
                        // rand%2 = 0 or 1 --> *2 = 0 or 2 --> -1 = -1 or 1
                        int vsign = ((std::rand() % 2) * 2) - 1;
                        // Generate a random voltage between -75 and 75
                        // Divide by 100 to create a float number
                        // Multiply by vsign to set it positive or negative
                        float v = ( (float) (std::rand() % 7500) / 100.0 ) * (float) vsign;
                        voltages.push_back( v );
                    }

                    std::memcpy(values_[start_idx], &voltages[0], a_.valuesize());
                }
            }

            if (a_.read()) std::memset(reads_[start_idx], 0, a_.valuesize());

            start_idx++;
        }
    }

#if 0
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            printf("thr %i:%i: ", i , j);
            printf("k = %s ; ", keys[i * a_.npairs() + j]);
            printf("v = %s \n", values[i * a_.npairs() + j]);
            printf("r = %s \n", reads[i * a_.npairs() + j]);
        }
    }
#endif

    printf("K/V init data: OK!\n");
}


void iobench::benchmark::write(stats & stats) {
    // In order to do random reads and make sure that each position is read exactly once,
    // create a permutation array and use its values to index the keys
    // The array is created here and the permutation is done at every iteration to increase
    // variability (only permuted if set by command line args)
    std::vector<int> wr_perm;
    wr_perm.reserve(a_.threads() * a_.npairs());
    for (int i = 0; i < a_.threads() * a_.npairs(); i++) {
        wr_perm.push_back(i);
    }

    struct timeval wr_start, wr_stop;

    // Compute the total amount of data to be written and the IOPS
    stats.mb() = (double) a_.threads() * (double) ( (a_.valuesize() * a_.npairs()) / (1024.0 * 1024.0) );
    stats.ops() = a_.npairs() * a_.threads();

    int wr_viter = 0; // valid iterations = iterations without errors
    for (int it = 0; it < a_.niter(); it++) {
        for (unsigned int i = 0; i < db_.size(); i++) {
            //db[i]->deleteDB();
        }

        std::vector< std::vector<KVStatus *> > status;
        for (unsigned int i = 0; i < db_.size(); i++) {
            std::vector<KVStatus *> s;
            db_[i]->createKVStatus(a_.npairs(), s);
            status.push_back(s);
        }

        // Create a different permutation
        if (a_.rnd_wr())
            std::random_shuffle (wr_perm.begin(), wr_perm.end());

        gettimeofday(&wr_start, NULL);
        #pragma omp parallel default(shared) //shared(keys_, values_, wr_perm, status) not allowed since there are class members
        {
            int id = omp_get_thread_num();
            /////////////////////////////////////////////
            int start_idx = id * a_.npairs();
            for (int i = 0; i < a_.npairs(); i++) {
                db_[id]->putKV(status[id][i], keys_[wr_perm[start_idx + i]], a_.keysize(), values_[wr_perm[start_idx + i]], a_.valuesize());
            }
            db_[id]->waitKVput(status[id], 0, a_.npairs());
            /////////////////////////////////////////////
        }
        gettimeofday(&wr_stop, NULL);

        int error = 0;
        for (int t = 0; t < a_.threads(); t++)
            for (int i = 0; i < a_.npairs(); i++)
                if (!status[t][i]->success())
                    error++;

        if (it >= a_.skip() && error == 0) {
            double sec = (wr_stop.tv_sec - wr_start.tv_sec) + (wr_stop.tv_usec - wr_start.tv_usec) * 1e-6;
            stats.record(sec);
            wr_viter++;
#ifdef IO_MPI
            if (a_.rank() == 0)
                printf("Iteration time: %g\n", sec);
#endif
        }
        errors_ += error;
    }
    std::cout << "K/V write data: OK!" << std::endl;

    // WRITE stats
    stats.compute_stats(a_.rank(), a_.procs());
}


void iobench::benchmark::read(stats & stats) {
    // Cassandra fails for high number of reads + many threads (seg fault inside waitKV)
    // One solution is to serialize the wait phase; the other is to reduce the number
    // of npairs...
    if (a_.backend() == "cassandra") {
        if (a_.threads() > 7)
            a_.npairs() /= 2; // max 1024
        if (a_.threads() > 15)
            a_.npairs() /= 2; // max 512
        if (a_.threads() > 23)
            a_.npairs() /= 2; // max 256
    }

    // In order to do random reads and make sure that each position is read exactly once,
    // create a permutation array and use its values to index the keys
    // The array is created here and the permutation is done at every iteration to increase
    // variability (only permuted if set by command line args)
    std::vector<int> rd_perm;
    rd_perm.reserve(a_.threads() * a_.npairs());
    for (int i = 0; i < a_.threads() * a_.npairs(); i++) {
        rd_perm.push_back(i);
    }

    struct timeval rd_start, rd_stop;

    // Compute the total amount of data to be read and the IOPS
    stats.mb() = (double) a_.threads() * (double) ( (a_.valuesize() * a_.npairs()) / (1024.0 * 1024.0) );
    stats.ops() = a_.npairs() * a_.threads();


    int rd_viter = 0; // valid iterations = iterations without errors
    for (int it = 0; it < 1 /*niter*/; it++) {
        std::vector< std::vector<KVStatus *> > status;
        for (unsigned int i = 0; i < db_.size(); i++) {
            std::vector<KVStatus *> s;
            db_[i]->createKVStatus(1 * a_.npairs(), s);
            status.push_back(s);
        }

        // Create a different permutation
        if (a_.rnd_rd())
            std::random_shuffle (rd_perm.begin(), rd_perm.end());

        gettimeofday(&rd_start, NULL);
#pragma omp parallel default(shared) //shared(keys_, reads_, rd_perm, status) not allowed since there are class members
        {
            int id = omp_get_thread_num();
            /////////////////////////////////////////////
            int start_idx = id * a_.npairs();
            for (int i = 0; i < a_.npairs(); i++) {
                db_[id]->getKV(status[id][i], keys_[rd_perm[start_idx + i]], a_.keysize(), reads_[rd_perm[start_idx + i]], a_.valuesize());
            }
            db_[id]->waitKVget(status[id], 0, a_.npairs());
            /////////////////////////////////////////////
        }
        gettimeofday(&rd_stop, NULL);

        int error = 0;
        for (int t = 0; t < a_.threads(); t++)
            for (int i = 0; i < a_.npairs(); i++)
                if (!status[t][i]->success())
                    error++;

        if (/*it >= a_.skip() &&*/ error == 0) { // Only 1 iteration by now
            double sec = (rd_stop.tv_sec - rd_start.tv_sec) + (rd_stop.tv_usec - rd_start.tv_usec) * 1e-6;
            stats.record(sec);
            rd_viter++;
        }
        errors_ += error;
    }
    std::cout << "K/V read data: OK!" << std::endl;

    if (a_.write()) {
        // If data was written in this execution, check we actually read the same values
        int errval = 0;
        int limit = a_.valuesize() / sizeof(float);
        for (int id = 0; id < a_.threads(); id++) {
            int start_idx = id * a_.npairs();
            for (int i = 0; i < a_.npairs(); i++) {
                float * fvalues = (float *) values_[start_idx];
                float * freads = (float *) reads_[start_idx];
                for (int i = 0; i < limit; i++) {
                    if (fvalues[i] != freads[i]) {
                        errval++;
                        //std::cout << "Wrote val (" << fvalues[i] << ") BUT read val (" << freads[i] << ") at pos [" << i << "]!!!" << std::endl;
                    }
                }
            }
        }
        errors_ += errval;
        if (errval != 0)
            std::cout << "Found " << errval << " errors!" << std::endl;
        else
            std::cout << "Checked retrieved values: OK!" << std::endl;

    }

    // READ stats
    stats.compute_stats(a_.rank(), a_.procs());
}

