/*
 * Neuromapp - iobench.cpp, Copyright (c), 2015,
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

#ifdef IO_MPI
#include <mpi.h>
#endif
#include <omp.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <boost/program_options.hpp>

#include "backends/common.h"
#include "backends/basic.h"
#include "backends/map.h"
#include "utils/statistics.h"


namespace po = boost::program_options;
using boost::lexical_cast;

/** \fun main
    \brief main program of the miniapp, run by MPI+OMP or just OMP
    Measure R/W performance of different K/V stores
 */
int main(int argc, char* argv[]) {

#ifdef IO_MPI
    MPI_Init(&argc, &argv);
    int mpi_size, mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    std::stringstream ss;
    ss << "I'm rank " << mpi_rank << " in a world of " << mpi_size << std::endl;
    std::cout << ss.str();
#endif

    po::options_description options("Options");

    options.add_options()("help,h", "Produce help message")
            ("backend,b", po::value<std::string>()->default_value("ldb"), "K/V backend to use")
            ("pairs,n", po::value<unsigned int>()->default_value(1024), "Number of k/v pairs per thread and per iteration")
            ("its,i", po::value<unsigned int>()->default_value(10), "Number of iterations per thread")
            ("skip,s", po::value<unsigned int>()->default_value(0), "Number of initial iterations to discard")
            ("threads,t", po::value<unsigned int>()->default_value(1), "Number of threads to use")
            ("keySize,k", po::value<size_t>()->default_value(64), "Size of keys in bytes")
            ("valSize,v", po::value<size_t>()->default_value(1024), "Size of values in bytes")
            ("compress,c", po::value<unsigned int>()->default_value(1), "Compress data")
            ("read,r", po::value<unsigned int>()->default_value(1), "Read data")
            ("write,w", po::value<unsigned int>()->default_value(1), "Write data")
            ("rrd", po::value<unsigned int>()->default_value(1), "Read data randomly")
            ("rwr", po::value<unsigned int>()->default_value(1), "Write data randomly") ;

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, options), vm);
        po::notify(vm);
    } catch (const boost::program_options::error& e) {
        std::cerr << "Command line parse error: " << e.what() << std::endl
                << options << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help")) {
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

    // Threads
    int nthr = vm["threads"].as<unsigned int>();
#pragma omp parallel
    {
        nthr = omp_get_num_threads();
    }

    //printf("Running with %i thread%s\n", nthr, (nthr == 1 ? "" : "s"));

    //BaseKV * db = NULL;
    std::string b(vm["backend"].as<std::string>());
    std::vector<BaseKV *> db;
    bool single_db = true;

    if (!single_db) {
        for (int i = 0; i < nthr; i++) {
            db.push_back(createDB(b));
        }
    } else {
        BaseKV * base = createDB(b);
        for (int i = 0; i < nthr; i++) {
            db.push_back(base);
        }
    }

    bool compress = vm["compress"].as<unsigned int>() == 1;
    int npairs = vm["pairs"].as<unsigned int>();

#ifdef IO_MPI
    db[0]->initDB(compress, nthr, npairs, mpi_rank, mpi_size);
#else
    if (!single_db) {
        for (int i = 0; i < db.size(); i++) {
            db[i]->initDB(compress, i, nthr);
        }
    } else {
        db[0]->initDB(compress, nthr, npairs);
    }
#endif
    std::string backend = db[0]->getDBName();

    /***************** Init data *****************/

    // Timing and #iterations
    double mb = 0, wr_bw_mb = 0, wr_iops = 0, rd_bw_mb = 0, rd_iops = 0;

    bool write = vm["write"].as<unsigned int>() == 1;
    bool read = vm["read"].as<unsigned int>() == 1;


    int skip = vm["skip"].as<unsigned int>();
    int niter = skip + vm["its"].as<unsigned int>(); // Discard N first its for timing

    struct timeval wr_start, wr_stop, rd_start, rd_stop;
    double wr_total_ms = 0.0, rd_total_ms = 0.0;
    double * wr_times = (double *) malloc(niter * sizeof(double));
    double * rd_times = (double *) malloc(niter * sizeof(double));
    double wr_bw_dev, wr_iops_dev, rd_bw_dev, rd_iops_dev;
    double wr_bw_err, wr_iops_err, rd_bw_err, rd_iops_err;

    // Setting up k/v data
    size_t key_size = vm["keySize"].as<size_t>();
    size_t value_size = vm["valSize"].as<size_t>();
    size_t ins_ksize = 0;
    size_t ins_vsize = 0;
    char ** keys = (char **) malloc(nthr * npairs * sizeof(char *)); // vector containing pointers to keys
    char ** values = (char **) malloc(nthr * npairs * sizeof(char *)); // vector containing pointers to the values to be written in the DB
    char ** reads = (char **) malloc(nthr * npairs * sizeof(char *)); // vector containing pointers to the read values retrieved from the DB

    for (int i = 0; i < nthr * npairs; i++) {
        keys[i] = (char *) malloc(key_size);
        values[i] = (char *) malloc(value_size);
        reads[i] = (char *) malloc(value_size);
    }

    char chars[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                    '1','2','3','4','5','6','7','8','9','0','-','+','=',';',':',',','.','?','/','*','<','>','[',']','{','}'};

#pragma omp parallel shared(keys, values, reads)
    {
        int id = omp_get_thread_num();
        int start_idx = id * npairs;
        for (int i = 0; i < npairs; i++) {
            // Set keys
            sprintf(keys[start_idx], "%i_%i", id, i);

            if (backend == "cassandra") {
                // Haven't been able to figure out the proper way to store an array
                // of floats into Cassandra...
                // 'Blob' or 'bytes' may be the right way to go:
                // http://stackoverflow.com/questions/26805458/datastax-c-c-driver-for-apache-cassandra-blob-conversion-gcrypt-crypto-key
                std::vector<char> voltages;
                for (unsigned int j = 0; j < value_size; j++) {
                    int idx = std::rand() % sizeof(chars); // sizeof(chars) should be 52
                    voltages.push_back( chars[idx] );
                }
                std::memcpy(values[start_idx], &voltages[0], value_size);
                values[start_idx][value_size-1] = '\0';

            } else {
                // Set values: use realistic voltage values
                int numVoltages = (value_size / sizeof(float)) + 1;
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

                std::memcpy(values[start_idx], &voltages[0], value_size);
            }

            std::memset(reads[start_idx], 0, value_size);

            start_idx++;
        }
    }

#if 0
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            printf("thr %i:%i: ", i , j);
            printf("k = %s ; ", keys[i * npairs + j]);
            printf("v = %s \n", values[i * npairs + j]);
            printf("r = %s \n", reads[i * npairs + j]);
        }
    }
#endif

    printf("K/V init data: OK!\n");


    /***************** Write data *****************/

    // Launching WRITE queries
    if (write) {
        bool rand_wr = vm["rwr"].as<unsigned int>() == 1;

        // In order to do random reads and make sure that each position is read exactly once,
        // create a permutation array and use its values to index the keys
        // The array is created here and the permutation is done at every iteration to increase
        // variability (only permuted if set by command line args)
        std::vector<int> wr_perm;
        wr_perm.reserve(nthr * npairs);
        for (int i = 0; i < nthr * npairs; i++) {
            wr_perm.push_back(i);
        }

        int wr_viter = 0; // valid iterations = iterations without errors
        for (int it = 0; it < niter; it++) {
            for (unsigned int i = 0; i < db.size(); i++) {
                //db[i]->deleteDB();
            }

            std::vector< std::vector<KVStatus *> > status;
            for (unsigned int i = 0; i < db.size(); i++) {
                std::vector<KVStatus *> s;
                db[i]->createKVStatus(1 * npairs, s);
                status.push_back(s);
                //db->createKVStatus(nthr * npairs, status);
            }

            // Create a different permutation
            if (rand_wr)
                std::random_shuffle (wr_perm.begin(), wr_perm.end());

            std::cout << "going to write data" << std::endl;

            gettimeofday(&wr_start, NULL);
#pragma omp parallel shared(keys, values, wr_perm)
            {
                int id = omp_get_thread_num();
                /////////////////////////////////////////////
                int start_idx = id * npairs;
                for (int i = 0; i < npairs; i++) {
                    //db->putKV(status[start_idx + i], keys[wr_perm[start_idx + i]], key_size, values[wr_perm[start_idx + i]], value_size);
                    db[id]->putKV(status[id][i], keys[wr_perm[start_idx + i]], key_size, values[wr_perm[start_idx + i]], value_size);
                    ins_ksize += key_size;
                    ins_vsize += value_size;
                }
                //db->waitKVput(status[id], start_idx, start_idx + npairs);
                db[id]->waitKVput(status[id], 0, npairs);
                /////////////////////////////////////////////
//                printf("Inserted: %i B for keys and %i B for values\n", ins_ksize, ins_vsize);
            }
            gettimeofday(&wr_stop, NULL);

            std::cout << "data written!" << std::endl;

            int error = 0;
            /*
            for (int t = 0; t < nthr; t++) {
                int start_idx = t * NUM_CONCURRENT_REQUESTS;
                for (int i = 0; i < NUM_CONCURRENT_REQUESTS; i++) {
                    if (!status[start_idx + i].ok()) {
                        //printf("------- ERROR --------\n");
                        //printf("----------------------\n");
                        error++;
                    }
                }
            }
            */
            if (it >= skip && error == 0) {
                wr_times[wr_viter] = (wr_stop.tv_sec - wr_start.tv_sec) * 1e3 + (wr_stop.tv_usec - wr_start.tv_usec) * 1e-3;
#ifdef IO_MPI
                if (mpi_rank == 0)
                    printf("Iteration time: %g\n", wr_times[wr_viter]);
#endif
                wr_total_ms += wr_times[wr_viter];
                wr_viter++;
            }
        }
        printf("K/V write data: OK!\n");

        /***************** Stats *****************/

        // WRITE stats
        //wr_total_ms /= (double) wr_viter;
        //printf("Insertions took (avg of %d iters): %g ms\n", wr_viter, wr_total_ms );
        mb = (double) ( (value_size * npairs) / (1024.0 * 1024.0) );
        mb *= (double) nthr;
        wr_bw_mb = mb / (wr_total_ms * 1e-3);
        //printf("Avg WR BW: %g MB/s\n", wr_bw_mb);
        wr_iops = (double) (npairs * nthr) / (wr_total_ms * 1e-3);
        //printf("Avg WR IOPS: %g IOPS\n", wr_iops);

        std::vector<double> wr_bw_values, wr_iops_values;
        for (int i = 0; i < wr_viter; i++) {
            wr_bw_values.push_back(mb / (wr_times[i] * 1e-3));
            wr_iops_values.push_back((double) (npairs * nthr) / (wr_times[i] * 1e-3));
        }

        iobench::compute_statistics(wr_bw_values, wr_bw_mb, wr_bw_dev, wr_bw_err);
        iobench::compute_statistics(wr_iops_values, wr_iops, wr_iops_dev, wr_iops_err);
    }

    /***************** Read data *****************/

    // Launching READ queries
    if (read) {
        bool rand_rd = vm["rrd"].as<unsigned int>() == 1;

        // Cassandra fails for high number of reads + many threads (seg fault inside waitKV)
        // One solution is to serialize the wait phase; the other is to reduce the number
        // of npairs...
        if (backend == "cassandra") {
            if (nthr > 7)
                npairs /= 2; // max 1024
            if (nthr > 15)
                npairs /= 2; // max 512
            if (nthr > 23)
                npairs /= 2; // max 256
        }

        // In order to do random reads and make sure that each position is read exactly once,
        // create a permutation array and use its values to index the keys
        // The array is created here and the permutation is done at every iteration to increase
        // variability (only permuted if set by command line args)
        std::vector<int> rd_perm;
        rd_perm.reserve(nthr * npairs);
        for (int i = 0; i < nthr * npairs; i++) {
            rd_perm.push_back(i);
        }

        int rd_viter = 0; // valid iterations = iterations without errors
        for (int it = 0; it < 1 /*niter*/; it++) {
            //std::vector<KVStatus *> status;
            //db->createKVStatus(nthr * npairs, status);
            std::vector< std::vector<KVStatus *> > status;
            for (unsigned int i = 0; i < db.size(); i++) {
                std::vector<KVStatus *> s;
                db[i]->createKVStatus(1 * npairs, s);
                status.push_back(s);
            }

            // Create a different permutation
            if (rand_rd)
                std::random_shuffle (rd_perm.begin(), rd_perm.end());

            gettimeofday(&rd_start, NULL);
#pragma omp parallel shared(keys, reads, rd_perm, status)
            {
                int id = omp_get_thread_num();
                /////////////////////////////////////////////
                int start_idx = id * npairs;
                for (int i = 0; i < npairs; i++) {
                    //db->getKV(status[start_idx + i], keys[rd_perm[start_idx + i]], key_size, reads[rd_perm[start_idx + i]], value_size);
                    db[id]->getKV(status[id][i], keys[rd_perm[start_idx + i]], key_size, reads[rd_perm[start_idx + i]], value_size);
                }
                //db->waitKVget(status, start_idx, start_idx + npairs);
                db[id]->waitKVget(status[id], 0, npairs);
                /////////////////////////////////////////////
            }
            gettimeofday(&rd_stop, NULL);

            int error = 0;
/*
            for (int t = 0; t < nthr; t++) {
                int start_idx = t * NUM_CONCURRENT_REQUESTS;
                for (int i = 0; i < NUM_CONCURRENT_REQUESTS; i++) {
                    if (!status[start_idx + i].ok()) {
                        //printf("------- ERROR --------\n");
                        //printf("----------------------\n");
                        error++;
                    }
                }
            }
*/
            if (/*it != 0 &&*/ error == 0) {
                rd_times[rd_viter] = (rd_stop.tv_sec - rd_start.tv_sec) * 1e3 + (rd_stop.tv_usec - rd_start.tv_usec) * 1e-3;
                rd_total_ms += rd_times[rd_viter];
                rd_viter++;
            }
        }


        /***************** Read Check *****************/
        int errval = 0;
        int limit = value_size / sizeof(float);
        for (int id = 0; id < nthr; id++) {
            int start_idx = id * npairs;
            for (int i = 0; i < npairs; i++) {
                float * fvalues = (float *) values[start_idx];
                float * freads = (float *) reads[start_idx];
               for (int i = 0; i < limit; i++) {
                    if (fvalues[i] != freads[i]) {
                        errval++;
                        //std::cout << "Wrote val (" << fvalues[i] << ") BUT read val (" << freads[i] << ") at pos [" << i << "]!!!" << std::endl;
                    }
                }
            }
        }
        if (errval != 0)
            std::cout << "Found " << errval << " errors!" << std::endl;


        /***************** Read Stats *****************/

        // READ stats
        //rd_total_ms /= (double) rd_viter;
        //printf("Reads took (avg of %d iters): %g ms\n", viter, rd_total_ms );
        mb = (double) ( (value_size * npairs) / (1024.0 * 1024.0) );
        mb *= (double) nthr;
        rd_bw_mb = mb / (rd_total_ms * 1e-3);
        //printf("Avg RD BW: %g MB/s\n", rd_bw_mb);
        rd_iops = (double) (npairs * nthr) / (rd_total_ms * 1e-3);
        //printf("Avg RD IOPS: %g IOPS\n", rd_iops);

        // Std devs & std error
        // NOTE: It makes no sense if there is only 1 iteration by now... but the code is there
        // Average
        std::vector<double> rd_bw_values, rd_iops_values;
        for (int i = 0; i < rd_viter; i++) {
            rd_bw_values.push_back(mb / (rd_times[i] * 1e-3));
            rd_iops_values.push_back((double) (npairs * nthr) / (rd_times[i] * 1e-3));
        }

        iobench::compute_statistics(rd_bw_values, rd_bw_mb, rd_bw_dev, rd_bw_err);
        iobench::compute_statistics(rd_iops_values, rd_iops, rd_iops_dev, rd_iops_err);
    }

    /***************** Finish *****************/
    printf("K/V finish: OK!\n");
    //db->deleteDB();


#ifdef IO_MPI
    MPI_Barrier(MPI_COMM_WORLD);

    if (mpi_size > 1) {
        if (mpi_rank == 0) {
            std::vector<double> wr_bws, wr_iopss, rd_bws, rd_iopss;
            wr_bws.reserve(mpi_size);
            wr_iopss.reserve(mpi_size);
            rd_bws.reserve(mpi_size);
            rd_iopss.reserve(mpi_size);

            wr_bws.push_back(wr_bw_mb);
            wr_iopss.push_back(wr_iops);
            rd_bws.push_back(rd_bw_mb);
            rd_iopss.push_back(rd_iops);

            for (int i = 1; i < mpi_size; i++) {
                wr_bws.push_back(0.0);
                wr_iopss.push_back(0.0);
                rd_bws.push_back(0.0);
                rd_iopss.push_back(0.0);

                MPI_Status status;
                MPI_Recv(&wr_bws.back(), 1, MPI_DOUBLE, i, i * 100 + 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&wr_iopss.back(), 1, MPI_DOUBLE, i, i * 100 + 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&rd_bws.back(), 1, MPI_DOUBLE, i, i * 100 + 2, MPI_COMM_WORLD, &status);
                MPI_Recv(&rd_iopss.back(), 1, MPI_DOUBLE, i, i * 100 + 3, MPI_COMM_WORLD, &status);

                double time;
                MPI_Recv(&time, 1, MPI_DOUBLE, i, i * 100 + 4, MPI_COMM_WORLD, &status);
                wr_total_ms += time;
            }

            // Statistics
            mb = mb * mpi_size;

            iobench::compute_statistics(wr_bws, wr_bw_mb, wr_bw_dev, wr_bw_err);
            iobench::compute_statistics(wr_iopss, wr_iops, wr_iops_dev, wr_iops_err);
            iobench::compute_statistics(rd_bws, rd_bw_mb, rd_bw_dev, rd_bw_err);
            iobench::compute_statistics(rd_iopss, rd_iops, rd_iops_dev, rd_iops_err);

       } else {
            MPI_Send(&wr_bw_mb, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 0, MPI_COMM_WORLD);
            MPI_Send(&wr_iops, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 1, MPI_COMM_WORLD);
            MPI_Send(&rd_bw_mb, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 2, MPI_COMM_WORLD);
            MPI_Send(&rd_iops, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 3, MPI_COMM_WORLD);
            MPI_Send(&wr_total_ms, 1, MPI_DOUBLE, 0, mpi_rank * 100 + 4, MPI_COMM_WORLD);
       }
    }
#endif

#ifdef IO_MPI
    if (mpi_rank == 0) {
#endif
    //if (wr_viter == niter - 1 && rd_viter == niter - 1) {
    printf("%i, %s, %g, %g, %g, %g, %i, %g, %g, %g, %g, %g, %i, %g, %g\n", nthr, backend.c_str(), mb,
            wr_bw_mb, wr_bw_dev, wr_bw_err,
            (int) wr_iops, wr_iops_dev, wr_iops_err,
            rd_bw_mb, rd_bw_dev, rd_bw_err,
            (int) rd_iops, rd_iops_dev, rd_iops_err);
    printf("Aggregated time for all loops is: %g s\n", wr_total_ms * 1e-3);
    //}
#ifdef IO_MPI
    }
#endif

#ifdef IO_MPI
    MPI_Finalize();
#endif

    return 0;
}
