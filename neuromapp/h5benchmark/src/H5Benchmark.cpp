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
#include "imeutil.h"

using namespace std;
using namespace bbp::sonata;

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
 * Enumerate that defines the different I/O drivers available.
 */
typedef enum 
{
    H5DRV_POSIX = 0,  // POSIX driver
    H5DRV_MPIIO,      // MPI-IO driver
    H5DRV_POSIX_v2,   // POSIX driver v2
    H5DRV_MPIIO_v2,   // MPI-IO driver v2
} h5drv_t;

/**
 * Helper method to obtain the elapsed time between two measurements.
 */
double getElapsed(timeval_t &tv_0, timeval_t &tv_1)
{
    return (double)(tv_1.tv_sec - tv_0.tv_sec) +
            (tv_1.tv_usec - tv_0.tv_usec) * 0.000001;
}

const size_t nmech = 4000;
const size_t mla_size = 40;
const size_t indices_size = 40;

void launchPOSIX(const char* path)
{
    FILE *f = fopen(path, "w");

    fprintf(f, "%s\n", "CORENEURON v1.21");
    fprintf(f, "%d ngid\n", 1);
    fprintf(f, "%d n_real_gid\n", 2);
    fprintf(f, "%d nnode\n", 3);
    fprintf(f, "%d ndiam\n", 4);
    fprintf(f, "%d nmech\n", nmech);

    for (int i=0; i < nmech; ++i) {
        fprintf(f, "%d\n", (i*2)%16777216);
        fprintf(f, "%d\n", i>>1);
    }

    fprintf(f, "%d nidata\n", 0);
    fprintf(f, "%d nvdata\n", 1);
    fprintf(f, "%d nweight\n", 2);

    // data
    {
        size_t count = 6;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < count; i++)
        {
            size_t base = (i > 0) ? sizeof(double) : sizeof(int);
            fprintf(f, "chkpnt %d\n", i);
            fwrite(a[i], base, (size[i]/base), f);
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // mechanism data
    {
        size_t count = 5;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < mla_size; i++)
        {
            for (int i = 0; i < count; i++)
            {
                size_t base = (i % 2) ? sizeof(int) : sizeof(double);
                fprintf(f, "chkpnt %d\n", i);
                fwrite(a[i], base, (size[i]/base), f);
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // connections
    {
        size_t count = 4;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < count; i++)
        {
            size_t base = (i > 1) ? sizeof(int) : sizeof(double);
            fprintf(f, "chkpnt %d\n", i);
            fwrite(a[i], base, (size[i]/base), f);
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // special handling for BBCOREPOINTER
    fprintf(f, "%d bbcorepointer\n", 1);
    {
        size_t count = 2;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < mla_size; i++)
        {
            fprintf(f, "%d\n", i);
            fprintf(f, "%d\n%d\n", i+1, i-1);
            for (int i = 0; i < count; i++)
            {
                size_t base = (i % 2) ? sizeof(int) : sizeof(double);
                fprintf(f, "chkpnt %d\n", i);
                fwrite(a[i], base, (size[i]/base), f);
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // indices
    {
        size_t count = 2;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < indices_size; i++)
        {
            for (int i = 0; i < count; i++)
            {
                size_t base = (i % 2) ? sizeof(int) : sizeof(double);
                fprintf(f, "%d\n", i);
                fprintf(f, "%d\n", i+1);
                fprintf(f, "%d\n", i-1);
                fprintf(f, "%d\n", i>>1);
                fprintf(f, "chkpnt %d\n", i);
                fwrite(a[i], base, (size[i]/base), f);
                fprintf(f, "chkpnt %d\n", i);
                fwrite(a[i], base, (size[i]/base), f);
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // fsync(fileno(f));
    fclose(f);
}

void launchPOSIXv2(const char* path)
{
    void *buffer = malloc(1048576ULL * 1024 * 2);
    char *ptr = (char *)buffer;

    ptr += sprintf(ptr, "%s\n", "CORENEURON v1.21");
    ptr += sprintf(ptr, "%d ngid\n", 1);
    ptr += sprintf(ptr, "%d n_real_gid\n", 2);
    ptr += sprintf(ptr, "%d nnode\n", 3);
    ptr += sprintf(ptr, "%d ndiam\n", 4);
    ptr += sprintf(ptr, "%d nmech\n", nmech);

    for (int i=0; i < nmech; ++i) {
        ptr += sprintf(ptr, "%d\n", (i*2)%16777216);
        ptr += sprintf(ptr, "%d\n", i>>1);
    }

    ptr += sprintf(ptr, "%d nidata\n", 0);
    ptr += sprintf(ptr, "%d nvdata\n", 1);
    ptr += sprintf(ptr, "%d nweight\n", 2);

    // data
    {
        size_t count = 6;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < count; i++)
        {
            ptr += sprintf(ptr, "chkpnt %d\n", i);
            memcpy(ptr, a[i], size[i]);
            ptr += size[i];
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // mechanism data
    {
        size_t count = 5;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < mla_size; i++)
        {
            for (int i = 0; i < count; i++)
            {
                ptr += sprintf(ptr, "chkpnt %d\n", i);
                memcpy(ptr, a[i], size[i]);
                ptr += size[i];
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // connections
    {
        size_t count = 4;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < count; i++)
        {
            ptr += sprintf(ptr, "chkpnt %d\n", i);
            memcpy(ptr, a[i], size[i]);
            ptr += size[i];
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // special handling for BBCOREPOINTER
    ptr += sprintf(ptr, "%d bbcorepointer\n", 1);
    {
        size_t count = 2;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < mla_size; i++)
        {
            ptr += sprintf(ptr, "%d\n", i);
            ptr += sprintf(ptr, "%d\n%d\n", i+1, i-1);
            for (int i = 0; i < count; i++)
            {
                ptr += sprintf(ptr, "chkpnt %d\n", i);
                memcpy(ptr, a[i], size[i]);
                ptr += size[i];
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // indices
    {
        size_t count = 2;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < indices_size; i++)
        {
            for (int i = 0; i < count; i++)
            {
                size_t base = (i % 2) ? sizeof(int) : sizeof(double);
                ptr += sprintf(ptr, "%d\n", i);
                ptr += sprintf(ptr, "%d\n", i+1);
                ptr += sprintf(ptr, "%d\n", i-1);
                ptr += sprintf(ptr, "%d\n", i>>1);
                ptr += sprintf(ptr, "chkpnt %d\n", i);
                memcpy(ptr, a[i], size[i]);
                ptr += size[i];
                ptr += sprintf(ptr, "chkpnt %d\n", i);
                memcpy(ptr, a[i], size[i]);
                ptr += size[i];
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    FILE *f = fopen(path, "w");
    fwrite(buffer, sizeof(char), ((uintptr_t)ptr - (uintptr_t)buffer), f);
    // fsync(fileno(f));
    fclose(f);
}

void launchMPIIO(const char* path, MPI_Info &info)
{
    char buffer[1048576];
    MPI_Offset nbytes = 0;
    MPI_File f;
    
    MPI_File_open(MPI_COMM_SELF, path, MPI_MODE_WRONLY | MPI_MODE_CREATE, info, &f);
    // MPI_File_seek(f, 0, SEEK_END);

    nbytes = sprintf(buffer, "%s\n", "CORENEURON v1.21");
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
    nbytes = sprintf(buffer, "%d ngid\n", 1);
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
    nbytes = sprintf(buffer, "%d n_real_gid\n", 2);
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
    nbytes = sprintf(buffer, "%d nnode\n", 3);
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
    nbytes = sprintf(buffer, "%d ndiam\n", 4);
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
    nbytes = sprintf(buffer, "%d nmech\n", nmech);
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);

    for (int i=0; i < nmech; ++i) {
        nbytes = sprintf(buffer, "%d\n", (i*2)%16777216);
        MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
        nbytes = sprintf(buffer, "%d\n", i>>1);
        MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
    }

    nbytes = sprintf(buffer, "%d nidata\n", 0);
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
    nbytes = sprintf(buffer, "%d nvdata\n", 1);
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
    nbytes = sprintf(buffer, "%d nweight\n", 2);
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);

    // data
    {
        size_t count = 6;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < count; i++)
        {
            nbytes = sprintf(buffer, "chkpnt %d\n", i);
            MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
            MPI_File_write(f, a[i], size[i], MPI_BYTE, MPI_STATUS_IGNORE);
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // mechanism data
    {
        size_t count = 5;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < mla_size; i++)
        {
            for (int i = 0; i < count; i++)
            {
                nbytes = sprintf(buffer, "chkpnt %d\n", i);
                MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
                MPI_File_write(f, a[i], size[i], MPI_BYTE, MPI_STATUS_IGNORE);
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // connections
    {
        size_t count = 4;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < count; i++)
        {
            nbytes = sprintf(buffer, "chkpnt %d\n", i);
            MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
            MPI_File_write(f, a[i], size[i], MPI_BYTE, MPI_STATUS_IGNORE);
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // special handling for BBCOREPOINTER
    nbytes = sprintf(buffer, "%d bbcorepointer\n", 1);
    MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
    {
        size_t count = 2;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < mla_size; i++)
        {
            nbytes = sprintf(buffer, "%d\n", i);
            MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
            nbytes = sprintf(buffer, "%d\n%d\n", i+1, i-1);
            MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
            for (int i = 0; i < count; i++)
            {
                nbytes = sprintf(buffer, "chkpnt %d\n", i);
                MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
                MPI_File_write(f, a[i], size[i], MPI_BYTE, MPI_STATUS_IGNORE);
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // indices
    {
        size_t count = 2;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < indices_size; i++)
        {
            for (int i = 0; i < count; i++)
            {
                nbytes = sprintf(buffer, "%d\n", i);
                MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
                nbytes = sprintf(buffer, "%d\n", i+1);
                MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
                nbytes = sprintf(buffer, "%d\n", i-1);
                MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
                nbytes = sprintf(buffer, "%d\n", i>>1);
                MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
                nbytes = sprintf(buffer, "chkpnt %d\n", i);
                MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
                MPI_File_write(f, a[i], size[i], MPI_BYTE, MPI_STATUS_IGNORE);
                nbytes = sprintf(buffer, "chkpnt %d\n", i);
                MPI_File_write(f, buffer, nbytes, MPI_BYTE, MPI_STATUS_IGNORE);
                MPI_File_write(f, a[i], size[i], MPI_BYTE, MPI_STATUS_IGNORE);
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // MPI_File_sync(f);
    MPI_File_close(&f);
}

void launchMPIIOv2(const char* path, MPI_Info &info)
{
    void *buffer = malloc(1048576ULL * 1024 * 2);
    char *ptr = (char *)buffer;

    ptr += sprintf(ptr, "%s\n", "CORENEURON v1.21");
    ptr += sprintf(ptr, "%d ngid\n", 1);
    ptr += sprintf(ptr, "%d n_real_gid\n", 2);
    ptr += sprintf(ptr, "%d nnode\n", 3);
    ptr += sprintf(ptr, "%d ndiam\n", 4);
    ptr += sprintf(ptr, "%d nmech\n", nmech);

    for (int i=0; i < nmech; ++i) {
        ptr += sprintf(ptr, "%d\n", (i*2)%16777216);
        ptr += sprintf(ptr, "%d\n", i>>1);
    }

    ptr += sprintf(ptr, "%d nidata\n", 0);
    ptr += sprintf(ptr, "%d nvdata\n", 1);
    ptr += sprintf(ptr, "%d nweight\n", 2);

    // data
    {
        size_t count = 6;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < count; i++)
        {
            ptr += sprintf(ptr, "chkpnt %d\n", i);
            memcpy(ptr, a[i], size[i]);
            ptr += size[i];
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // mechanism data
    {
        size_t count = 5;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < mla_size; i++)
        {
            for (int i = 0; i < count; i++)
            {
                ptr += sprintf(ptr, "chkpnt %d\n", i);
                memcpy(ptr, a[i], size[i]);
                ptr += size[i];
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // connections
    {
        size_t count = 4;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < count; i++)
        {
            ptr += sprintf(ptr, "chkpnt %d\n", i);
            memcpy(ptr, a[i], size[i]);
            ptr += size[i];
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // special handling for BBCOREPOINTER
    ptr += sprintf(ptr, "%d bbcorepointer\n", 1);
    {
        size_t count = 2;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < mla_size; i++)
        {
            ptr += sprintf(ptr, "%d\n", i);
            ptr += sprintf(ptr, "%d\n%d\n", i+1, i-1);
            for (int i = 0; i < count; i++)
            {
                ptr += sprintf(ptr, "chkpnt %d\n", i);
                memcpy(ptr, a[i], size[i]);
                ptr += size[i];
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    // indices
    {
        size_t count = 2;
        void **a = (void **)malloc(sizeof(void *) * count);
        size_t *size = (size_t *)malloc(sizeof(size_t) * count);
        for (int i = 0; i < count; i++)
        {
            size[i] = (1048576 << ((i + (count >> 1)) % count)) % 16777216;
            a[i] = malloc(size[i]);
            memset(a[i], i+21, size[i]);
        }
        for (int i = 0; i < indices_size; i++)
        {
            for (int i = 0; i < count; i++)
            {
                size_t base = (i % 2) ? sizeof(int) : sizeof(double);
                ptr += sprintf(ptr, "%d\n", i);
                ptr += sprintf(ptr, "%d\n", i+1);
                ptr += sprintf(ptr, "%d\n", i-1);
                ptr += sprintf(ptr, "%d\n", i>>1);
                ptr += sprintf(ptr, "chkpnt %d\n", i);
                memcpy(ptr, a[i], size[i]);
                ptr += size[i];
                ptr += sprintf(ptr, "chkpnt %d\n", i);
                memcpy(ptr, a[i], size[i]);
                ptr += size[i];
            }
        }
        for (int i = 0; i < count; i++)
        {
            free(a[i]);
        }
        free(a);
        free(size);
    }

    MPI_File f;
    MPI_File_open(MPI_COMM_SELF, path, MPI_MODE_WRONLY | MPI_MODE_CREATE, info, &f);
    MPI_File_write(f, buffer, ((uintptr_t)ptr - (uintptr_t)buffer), MPI_BYTE, MPI_STATUS_IGNORE);
    // MPI_File_sync(f);
    MPI_File_close(&f);
}

int main(int argc, char **argv)
{
    h5drv_t drv = (h5drv_t)(argc-2);
    int rank = 0;
    vector<timeval_t> tv(2);
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&tv[0], NULL);
    // for (int i = 0; i < 4; i++)
    {
        char path[4096];
        sprintf(path, "%s/test_%d.dat", argv[1], rank);
        const auto& path_info = IMEUtil::getPathInfo(path);
        MPI_Info info = MPI_INFO_NULL;
    
        // Set proper MPI-IO hints for better IME support
        if ((drv == H5DRV_MPIIO || drv == H5DRV_MPIIO_v2) && path_info.first == FSTYPE_IME) {
            IMEUtil::setMPIHints(info);
            strcpy(path, path_info.second.c_str());
        }

        switch (drv)
        {
            case H5DRV_POSIX:
                launchPOSIX(path); break;
            case H5DRV_POSIX_v2:
                launchPOSIXv2(path); break;
            case H5DRV_MPIIO:
                launchMPIIO(path, info); break;
            case H5DRV_MPIIO_v2:
                launchMPIIOv2(path, info); break;
            default:;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&tv[1], NULL);

    // Output the measurements
    if (rank == 0)
    {
        printf("%s: %lfs\n", (drv == H5DRV_POSIX)    ? "POSIX"    :
                             (drv == H5DRV_POSIX_v2) ? "POSIX v2" :
                             (drv == H5DRV_MPIIO)    ? "MPI-IO"   :
                                                       "MPI-IO v2",
                             getElapsed(tv[0], tv[1]));
    }

    // Release resources
    MPI_Finalize();

    return 0;
}
