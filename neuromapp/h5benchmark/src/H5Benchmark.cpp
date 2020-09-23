#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <mpi.h>
#include "H5Parser.hpp"
#include "H5Util.hpp"
#include "IOApi_HDF5.hpp"
#include "IOApi_Hi5.hpp"
#include "IOApi_MKit.hpp"
#include "IOApi_MPIO.hpp"

using namespace std;
using namespace h5benchmark;

#define INPUT_PARAMS "[bmark] [api] [drv] [factor] [use_boost] [file]"

typedef struct timeval timeval_t;

/**
 * Structure that represents a performance measurement.
 */
typedef struct _measurement_t
{
    string description;
    double elapsed;
    double avg;
    double min;
    double max;
    
    _measurement_t(string _description, double _elapsed)
        : description(_description)
        , elapsed(_elapsed)
        , avg(0.0)
        , min(0.0)
        , max(0.0) { }
} measurement_t;

/**
 * Enumerate that defines the different benchmark types available.
 */
typedef enum 
{
    H5BMARK_SEQ = 0,  // Sequential benchmark
    H5BMARK_RND,      // Pseudo-random benchmark
} h5bmark_t;

/**
 * Enumerate that defines the different APIs available to read the datasets.
 */
typedef enum 
{
    H5API_DEFAULT = 0,  // Default C API
    H5API_HIGHFIVE,     // BBP HighFive C++ wrapper
    H5API_MORPHOKIT,    // BBP Morpho-kit reader
    H5API_MPIO,         // Custom MPI-IO reader
} h5api_t;

/**
 * Enumerate that defines the different I/O drivers available.
 */
typedef enum 
{
    H5DRV_POSIX = 0,  // POSIX HDF5 driver
    H5DRV_MPIIO,      // MPI-IO pHDF5 driver
} h5drv_t;

/**
 * Sequential / Random benchmark that retrieves datasets from each group.
 */
int launchBenchmark(h5bmark_t type, IOApi *ioapi, double factor,
                    vector<group_t> groups, int rank)
{
    const size_t num_groups = groups.size(); 
    const size_t rd_limit   = num_groups * factor;
    const int    is_random  = (type == H5BMARK_RND);
    off_t        offset     = 0;
    uint32_t     seed       = (rank + 1) * 921;
    
    if (is_random)
    {
        // Set the seed and generate the first offset
        rand_r(&seed);
        offset = (rand_r(&seed) % num_groups);
    }
    
    for (size_t rd_count = 0; rd_count < rd_limit; rd_count++)
    {
        ioapi->readGroup(groups[offset]);
        
        offset = ((is_random) ? (off_t)rand_r(&seed) :
                                (offset + 1)) % num_groups;
    }
    
    return 0;
}

/**
 * Helper method to obtain the elapsed time between two measurements.
 */
double getElapsed(timeval_t &tv_0, timeval_t &tv_1)
{
    return (double)(tv_1.tv_sec - tv_0.tv_sec) +
            (tv_1.tv_usec - tv_0.tv_usec) * 0.000001;
}

int main(int argc, char **argv)
{
    h5bmark_t      bmark       = H5BMARK_SEQ;
    h5api_t        api         = H5API_DEFAULT;
    h5drv_t        drv         = H5DRV_POSIX;
    double         factor      = 1.0;
    int            use_boost   = 0;
    char           *path       = NULL;
    int            rank        = 0;
    int            nranks      = 0;
    IOApi          *ioapi      = nullptr;
    vector<group_t>     groups;
    vector<timeval_t>   tv(4);
    list<measurement_t> measurements;
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    // Check if the number of parameters match the expected
    if (argc != 7)
    {
        cerr << "Error: The number of parameters is incorrect!" << endl;
        cerr << "Use: " << argv[0] << " " << INPUT_PARAMS << endl;
        return -1;
    }
    
    // Retrieve the benchmark settings and check if the file is correct
    sscanf(argv[1], "%d", (int *)&bmark);
    sscanf(argv[2], "%d", (int *)&api);
    sscanf(argv[3], "%d", (int *)&drv);
    sscanf(argv[4], "%lf", &factor);
    sscanf(argv[5], "%d", (int *)&use_boost);
    
    if (strncmp((path = argv[6]), "ime:", 4) && access(path, F_OK))
    {
        cerr << "Error: File does not exist or cannot be accessed." << endl;
        return -1;
    }

    // Retrieve all the groups inside the file and share the information
    {
        size_t groups_cnt = 0;
        
        if (rank == 0)
        {
            gettimeofday(&tv[0], NULL);
            H5Parser parser(string(path), (drv == H5DRV_MPIIO));
            parser.getGroups(groups);
            gettimeofday(&tv[1], NULL);

            printf("Group Retrieval (s): %lf", getElapsed(tv[0], tv[1]));
            
            groups_cnt = groups.size();
        }
        
        MPI_Bcast(&groups_cnt, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
        groups.resize(groups_cnt);
        MPI_Bcast(&groups[0], groups_cnt * sizeof(group_t), MPI_BYTE, 0,
                  MPI_COMM_WORLD);
    }
    
    // Configure the API for I/O
    gettimeofday(&tv[0], NULL);
    gettimeofday(&tv[2], NULL);
    switch (api)
    {
        case H5API_HIGHFIVE:
            ioapi = new IOApiH5(string(path), (drv == H5DRV_MPIIO), use_boost);
            break;
        case H5API_MORPHOKIT:
            ioapi = new IOApiMKit(string(path), (drv == H5DRV_MPIIO)); break;
        case H5API_MPIO:
            ioapi = new IOApiMPIO(string(path), (drv == H5DRV_MPIIO)); break;
        default:
            ioapi = new IOApiHDF5(string(path), (drv == H5DRV_MPIIO));
    }
    gettimeofday(&tv[1], NULL);

    measurements.push_back(measurement_t("API Creation", getElapsed(tv[0], tv[1])));

    // Launch the benchmark that reads the groups from the file
    gettimeofday(&tv[0], NULL);
    launchBenchmark(bmark, ioapi, factor, groups, rank);
    gettimeofday(&tv[1], NULL);

    measurements.push_back(measurement_t("Benchmark", getElapsed(tv[0], tv[1])));

    MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&tv[3], NULL);

    // Output the measurements
    if (rank == 0)
    {
        printf("Execution Time (s): %lf", getElapsed(tv[2], tv[3]));
    }

    for (auto &m : measurements) {
        MPI_Reduce(&m.elapsed, &m.avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&m.elapsed, &m.min, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
        MPI_Reduce(&m.elapsed, &m.max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        
        if (rank == 0)
        {
            printf("  > %s (s): %lf %lf %lf", m.description,
                                              (m.avg / (double)nranks),
                                              m.min,
                                              m.max);
        }
    }

    // Release resources
    delete ioapi;
    MPI_Finalize();

    return 0;
}
