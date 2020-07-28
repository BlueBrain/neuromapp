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
#include <hdf5.h>
#include <mpi.h>
#include <highfive/H5DataSet.hpp>
#include <highfive/H5Group.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>

using namespace std;
using namespace HighFive;

#define INPUT_PARAMS    "[bmark] [api] [drv] [file]"
#define BUFFER_SIZE     (1048576 * 256) // TODO: Check for dataset sizes!
#define CACHE_SIZE_STEP 1024
#define NAME_LENGTH_MAX 256

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
} h5api_t;

/**
 * Enumerate that defines the different I/O drivers available.
 */
typedef enum 
{
    H5DRV_POSIX = 0,  // POSIX HDF5 driver
    H5DRV_MPIIO,      // MPI-IO pHDF5 driver
} h5drv_t;

/*
 * Data structure that represents a group inside a file.
 */
typedef struct
{
    hid_t loc_id;                                // Group identifier (obj. loc.)
    char  name[NAME_LENGTH_MAX - sizeof(hid_t)]; // Name of the group
} h5group_t;

/*
 * Container that stores the information of the groups located in a file.
 */
static struct
{
    h5group_t *data;
    size_t    count;
    size_t    size;
} g_h5groups = { 0 };

/**
 * Helper method to update the container that stores the groups.
 */
void add_group(hid_t loc_id, const char *name)
{
    if (g_h5groups.count == g_h5groups.size)
    {
        g_h5groups.size += CACHE_SIZE_STEP;
        g_h5groups.data  = (h5group_t *)realloc(g_h5groups.data,
                                           sizeof(h5group_t) * g_h5groups.size);
    }
    
    g_h5groups.data[g_h5groups.count].loc_id = loc_id;
    strcpy(g_h5groups.data[g_h5groups.count++].name, name);
}

/**
 * Operator function to search for all the groups using H5Literate.
 */
herr_t op_func(hid_t loc_id, const char *name, const H5L_info_t *info,
               void *operator_data)
{
    H5O_info_t infobuf = { 0 };

    // Get type of the object with the provided name
    if (!H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT) &&
        infobuf.type == H5O_TYPE_GROUP)
    {
        add_group(loc_id, name);
        
        return H5Literate_by_name(loc_id, name, H5_INDEX_NAME, H5_ITER_NATIVE,
                                  NULL, op_func, NULL, H5P_DEFAULT);
    }
    
    return 0;
}

/**
 * Helper method to read the content of a group using the default C API.
 */
int read_group(h5drv_t drv, hid_t file, h5group_t *group)
{
    static void *buffer    = NULL;
    hid_t       gid        = 0;
    hid_t       did_struct = 0;
    hid_t       did_points = 0;
    hid_t       plistid    = H5P_DEFAULT;
    
    if (buffer == NULL)
    {
        buffer = malloc(BUFFER_SIZE);
    }
    
    // Retrieve the group and open the datasets
    gid        = H5Gopen2(file, group->name, H5P_DEFAULT);
    did_struct = H5Dopen2(gid, "structure", H5P_DEFAULT);
    did_points = H5Dopen2(gid, "points", H5P_DEFAULT);

    // Configure the MPI-IO driver for pHDF5, if needed
    if (drv == H5DRV_MPIIO)
    {
        plistid = H5Pcreate(H5P_DATASET_XFER);
        H5Pset_dxpl_mpio(plistid, H5FD_MPIO_INDEPENDENT);
        // Alternative: H5FD_MPIO_COLLECTIVE
    }
    
    // Read the content available on each dataset
    H5Dread(did_struct, H5T_NATIVE_INT,    H5S_ALL, H5S_ALL, plistid, buffer);
    H5Dread(did_points, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, plistid, buffer);
    
    // printf ("Dataset: %s\n    (%lf,%lf,%lf,%lf)\n", group->name,
    //                                                 ((double*)buffer)[0],
    //                                                 ((double*)buffer)[1],
    //                                                 ((double*)buffer)[2],
    //                                                 ((double*)buffer)[3]);
    
    H5Dclose(did_struct);
    H5Dclose(did_points);
    H5Gclose(gid);
    
    return 0;
}

/**
 * Helper method to read the content of a group using the HighFive API.
 */
int read_group_h5(h5drv_t drv, File *file_h5, h5group_t *group)
{
    // Retrieve the group and open the datasets
    Group                 group_h5    = file_h5->getGroup(string(group->name));
    DataSet               dset_struct = group_h5.getDataSet("structure");
    DataSet               dset_points = group_h5.getDataSet("points");
    vector<vector<int>>   buffer_struct;
    vector<vector<float>> buffer_points;
    
    // Read the content available on each dataset
    dset_struct.read(buffer_struct);
    dset_points.read(buffer_points);
    
    // printf ("Dataset: %s\n    (%lf,%lf,%lf,%lf)\n", group->name,
    //                                                 buffer_points[0][0],
    //                                                 buffer_points[0][1],
    //                                                 buffer_points[0][2],
    //                                                 buffer_points[0][3]);
    
    return 0;
}

/**
 * Sequential / Random benchmark that retrieves datasets from each group.
 */
int launch_bmark(h5bmark_t bmark, h5api_t api, h5drv_t drv, hid_t file,
                 File *file_h5, int rank)
{
    const int is_random = (bmark == H5BMARK_RND);
    off_t     offset    = 0;
    uint32_t  seed      = (rank + 1) * 921;
    
    if (is_random)
    {
        // Set the seed and generate the first offset
        rand_r(&seed);
        offset = (rand_r(&seed) % g_h5groups.count);
    }
    
    for (size_t rd_count = 0; rd_count < g_h5groups.count; rd_count++)
    {
        if (api == H5API_DEFAULT)
        {
            read_group(drv, file, &g_h5groups.data[offset]);
        }
        else
        {
            read_group_h5(drv, file_h5, &g_h5groups.data[offset]);
        }
        
        offset = ((is_random) ? (off_t)rand_r(&seed) :
                                (offset + 1)) % g_h5groups.count;
    }
    
    return 0;
}

int main(int argc, char **argv)
{
    h5bmark_t bmark     = H5BMARK_SEQ;
    h5api_t   api       = H5API_DEFAULT;
    h5drv_t   drv       = H5DRV_POSIX;
    char      *path     = NULL;
    hid_t     file      = 0;
    File      *file_h5  = nullptr;
    hid_t     fapl_id   = H5P_DEFAULT;
    int       rank      = 0;
    int       num_ranks = 0;
    
    // Check if the number of parameters match the expected
    if (argc != 5)
    {
        fprintf(stderr, "Error: The number of parameters is incorrect!\n");
        fprintf(stderr, "Use: %s %s\n", argv[0], INPUT_PARAMS);
        return -1;
    }
    
    // Retrieve the benchmark settings and check if the file is correct
    sscanf(argv[1], "%d", (int *)&bmark);
    sscanf(argv[2], "%d", (int *)&api);
    sscanf(argv[3], "%d", (int *)&drv);
    
    if (access((path = argv[4]), F_OK))
    {
        fprintf(stderr, "Error: File does not exist or cannot be accessed.\n");
        return -1;
    }
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

    // Open file and force the first rank to look for all the groups inside
    if (rank == 0 || api == H5API_DEFAULT)
    {
        // Enable the MPI-IO driver for pHDF5, if needed
        if (drv == H5DRV_MPIIO)
        {
            fapl_id = H5Pcreate(H5P_FILE_ACCESS);
            H5Pset_fapl_mpio(fapl_id, MPI_COMM_WORLD, MPI_INFO_NULL);
        }
        
        file = H5Fopen(path, H5F_ACC_RDONLY, fapl_id);
    }

    if (api == H5API_HIGHFIVE)
    {
        try
        {
            file_h5 = new File(path, File::ReadOnly, // <<<<<<<<<<<< pHDF5!!!!
                           MPIOFileDriver(MPI_COMM_WORLD, MPI_INFO_NULL));
        }
        catch (Exception& err) { MPI_Abort(MPI_COMM_WORLD, 1); }
    }
    
    if (rank == 0)
    {
        H5Literate(file, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, op_func, NULL);
    }
    
    // Pre-allocate the space for the groups and share the information
    MPI_Bcast(&g_h5groups.count, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    g_h5groups.size = g_h5groups.count * sizeof(h5group_t); // Overwrite size
    g_h5groups.data = (h5group_t *)realloc(g_h5groups.data, g_h5groups.size);
    MPI_Bcast(g_h5groups.data, g_h5groups.size, MPI_BYTE, 0, MPI_COMM_WORLD);
    
    // Launch the benchmark that reads the complete file
    launch_bmark(bmark, api, drv, file, file_h5, rank);
    MPI_Barrier(MPI_COMM_WORLD);

    // Close and release resources
    if (rank == 0 || api == H5API_DEFAULT)
    {
        H5Fclose(file);
    }
    
    delete file_h5;

    free(g_h5groups.data);
    MPI_Finalize();

    return 0;
}
