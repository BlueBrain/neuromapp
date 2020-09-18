#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mpi.h>

#define INPUT_PARAMS "[bsize] [path]"

void init_read(int rank, int nranks, size_t bsize, double **buffer)
{
    const double factor = 1 / (double)nranks;
    
    *buffer = (double *)malloc(sizeof(double) * bsize);
    
    for (off_t i = 0; i < bsize; i++)
    {
        (*buffer)[i] = (double)i * factor;
    }
}

void release_read(double **buffer)
{
    free(*buffer);
    *buffer = NULL;
}

void launch_read(int rank, int nranks, size_t bsize, char *path, double *buffer)
{
    MPI_File     fh     = MPI_FILE_NULL;
    MPI_Datatype contig = MPI_DATATYPE_NULL;
    MPI_Datatype ftype  = MPI_DATATYPE_NULL;
    MPI_Offset   offset = rank * (sizeof(double) * bsize);
    
    MPI_File_open(MPI_COMM_WORLD, path, (MPI_MODE_CREATE | MPI_MODE_WRONLY), 
                  MPI_INFO_NULL, &fh);
                  
    MPI_File_write_at_all(fh, offset, buffer, bsize, MPI_DOUBLE, 
                          MPI_STATUS_IGNORE);
    
    MPI_File_close(&fh);
}

int main(int argc, char **argv)
{
    size_t bsize   = 0;
    char   *path   = NULL;
    double *buffer = NULL;
    int    rank    = 0;
    int    nranks  = 0;
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    // Check if the number of parameters match the expected
    if (argc != 3)
    {
        fprintf(stderr, "Error: The number of parameters is incorrect!\n");
        fprintf(stderr, "Use: %s %s\n", argv[0], INPUT_PARAMS);
        return -1;
    }
    
    // Retrieve the benchmark settings and check if the file is correct
    sscanf(argv[1], "%zu", &bsize);
    path = argv[2];
    
    // Launch the collective read
    {
        if (rank == 0)
            printf("Writing %zux8x%d blocks to \"%s\"\n", bsize, nranks, path);
        
        init_read(rank, nranks, bsize, &buffer);
        launch_read(rank, nranks, bsize, path, buffer);
        release_read(&buffer);
        
        if (rank == 0)
            printf("Done!\n");
    }

    // Release resources
    MPI_Finalize();

    return 0;
}
