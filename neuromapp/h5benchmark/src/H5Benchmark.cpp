#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <mpi.h>
#include "H5Parser.hpp"
#include "H5Util.hpp"
#include "IOApi_HDF5.hpp"
#include "IOApi_Hi5.hpp"
#include "IOApi_MKit.hpp"

using namespace std;
using namespace h5benchmark;

#define INPUT_PARAMS "[bmark] [api] [drv] [factor] [use_boost] [file]"

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
                    vector<string> groups, int rank)
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

int main(int argc, char **argv)
{
    h5bmark_t      bmark       = H5BMARK_SEQ;
    h5api_t        api         = H5API_DEFAULT;
    h5drv_t        drv         = H5DRV_POSIX;
    double         factor      = 1.0;
    int            use_boost   = 0;
    char           *path       = NULL;
    int            rank        = 0;
    IOApi          *ioapi      = nullptr;
    fixedstring_t  *groups_tmp = NULL;
    size_t         groups_cnt  = 0;
    size_t         groups_size = 0;
    vector<string> groups;
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

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
    
    if (access((path = argv[6]), F_OK))
    {
        cerr << "Error: File does not exist or cannot be accessed." << endl;
        return -1;
    }

    // Retrieve all the groups inside the file and share the information
    {
        if (rank == 0)
        {
            H5Parser parser(string(path), (drv == H5DRV_MPIIO));
            parser.getGroups(groups);
            
            groups_tmp = H5Util::vectorConv(groups);
            groups_cnt = groups.size();
        }
        
        MPI_Bcast(&groups_cnt, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
        groups_size = groups_cnt * sizeof(fixedstring_t);
        groups_tmp  = (fixedstring_t *)realloc(groups_tmp, groups_size);
        MPI_Bcast(groups_tmp, groups_size, MPI_BYTE, 0, MPI_COMM_WORLD);
        
        // Convert the buffer back to a vector of strings
        if (rank != 0)
        {
            groups = H5Util::bufferConv(groups_tmp, groups_cnt);
        }
        
        free(groups_tmp);
    }
    
    // Configure the API for I/O
    switch (api)
    {
        case H5API_HIGHFIVE:
            ioapi = new IOApiH5(string(path), (drv == H5DRV_MPIIO), use_boost);
            break;
        case H5API_MORPHOKIT:
            ioapi = new IOApiMKit(string(path), (drv == H5DRV_MPIIO)); break;
        default:
            ioapi = new IOApiHDF5(string(path), (drv == H5DRV_MPIIO));
    }
    
    // Launch the benchmark that reads the groups from the file
    launchBenchmark(bmark, ioapi, factor, groups, rank);
    MPI_Barrier(MPI_COMM_WORLD);

    // Release resources
    delete ioapi;
    MPI_Finalize();

    return 0;
}
