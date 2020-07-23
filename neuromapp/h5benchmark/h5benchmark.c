#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <hdf5.h>
#include <mpi.h>

#define INPUT_PARAMS    "[bmark] [api] [file]"
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
 * Enumerate that defines the different I/O APIs available.
 */
typedef enum 
{
    H5API_POSIX = 0,  // POSIX HDF5 driver
    H5API_MPIIO,      // MPI-IO pHDF5 driver
} h5api_t;

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
 * Helper method to read the content of a group into a temporary buffer.
 */
int read_group(h5api_t api, hid_t file, h5group_t *group)
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
    
    gid        = H5Gopen2(file, group->name, H5P_DEFAULT);
    did_struct = H5Dopen2(gid, "structure", H5P_DEFAULT);
    did_points = H5Dopen2(gid, "points", H5P_DEFAULT);

    // Configure the MPI-IO driver for pHDF5, if needed
    if (api == H5API_MPIIO)
    {
        plistid = H5Pcreate(H5P_DATASET_XFER);
        H5Pset_dxpl_mpio(plistid, H5FD_MPIO_INDEPENDENT);
        // Alternative: H5FD_MPIO_COLLECTIVE
    }
    
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
 * Sequential / Random benchmark that retrieves datasets from each group.
 */
int launch_bmark(h5bmark_t bmark, h5api_t api, hid_t file, int rank)
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
        read_group(api, file, &g_h5groups.data[offset]);
        
        offset = ((is_random) ? (off_t)rand_r(&seed) :
                                (offset + 1)) % g_h5groups.count;
    }
    
    return 0;
}

int main(int argc, char **argv)
{
    h5bmark_t bmark     = 0;
    h5api_t   api       = 0;
    char      *path     = argv[1];
    hid_t     file      = 0;
    hid_t     fapl_id   = H5P_DEFAULT;
    int       rank      = 0;
    int       num_ranks = 0;
    
    // Check if the number of parameters match the expected
    if (argc != 4)
    {
        fprintf(stderr, "Error: The number of parameters is incorrect!\n");
        fprintf(stderr, "Use: %s %s\n", argv[0], INPUT_PARAMS);
        return -1;
    }
    
    // Retrieve the benchmark settings and check if the file is correct
    sscanf(argv[1], "%d", (int *)&bmark);
    sscanf(argv[2], "%d", (int *)&api);
    
    if (access((path = argv[3]), F_OK))
    {
        fprintf(stderr, "Error: File does not exist or cannot be accessed.\n");
        return -1;
    }
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
    
    // Enable the MPI-IO driver for pHDF5, if needed
    if (api == H5API_MPIIO)
    {
        fapl_id = H5Pcreate(H5P_FILE_ACCESS);
        H5Pset_fapl_mpio(fapl_id, MPI_COMM_WORLD, MPI_INFO_NULL);
    }

    // Open file and force the first rank to look for all the groups inside
    file = H5Fopen(path, H5F_ACC_RDONLY, fapl_id);
    
    if (rank == 0)
    {
        H5Literate(file, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, op_func, NULL);
    }
    
    // Pre-allocate the space for the groups and share the information
    MPI_Bcast(&g_h5groups.count, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    
    if (rank != 0)
    {
        g_h5groups.data = malloc(g_h5groups.count * sizeof(h5group_t));
    }
    
    MPI_Bcast(g_h5groups.data, g_h5groups.count * sizeof(h5group_t), MPI_BYTE, 
              0, MPI_COMM_WORLD);
    
    // Launch the benchmark that reads the complete file
    launch_bmark(bmark, api, file, rank);
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Close and release resources
    H5Fclose(file);

    MPI_Finalize();

    return 0;
}
