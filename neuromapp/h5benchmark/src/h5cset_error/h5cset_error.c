#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>

#define FILE    "test.h5"
#define DATASET "MyRandomDataset"

void write_dataset()
{
    char* wdata[1] = { "Hello there!" };
    hid_t file, filetype, space, dset;
    hsize_t dims = 1;

    // Create a new file using the default properties
    file = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    // Create the reference datatype with ASCII encoding
    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, H5T_VARIABLE);
    H5Tset_cset(filetype, H5T_CSET_ASCII);

    // Create dataset with a simple dataspace
    space = H5Screate_simple(1, &dims, NULL);
    dset = H5Dcreate(file, DATASET, filetype, space, H5P_DEFAULT,
                     H5P_DEFAULT, H5P_DEFAULT);

    // Write the string data to it
    H5Dwrite(dset, filetype, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);

    // Close and release resources
    H5Dclose(dset);
    H5Sclose(space);
    H5Tclose(filetype);
    H5Fclose(file);
}

void read_dataset(int8_t enable_error)
{
    char*      rdata[1] = { NULL };
    hid_t      file, memtype, space, dset;
    H5T_cset_t cset;
    int        test;

    // Open the reference file
    file = H5Fopen(FILE, H5F_ACC_RDONLY, H5P_DEFAULT);

    for (test = 0; test < 2; test++)
    {
        // Open the dataset
        dset = H5Dopen(file, DATASET, H5P_DEFAULT);

        // Create the memory datatype
        memtype = H5Tcopy(H5T_C_S1);
        H5Tset_size(memtype, H5T_VARIABLE);

        // Set the character set according to the test behaviour
        if (enable_error)
        {
            // FAILS: The character set of the first read does NOT match
            cset = (test == 0) ? H5T_CSET_UTF8 : H5T_CSET_ASCII;
        }
        else
        {
            // WORKS: The character set of the first read matches
            cset = (test == 0) ? H5T_CSET_ASCII : H5T_CSET_UTF8;
        }
        H5Tset_cset(memtype, cset);

        // Read the data from the file
        H5Dread(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    
        printf("Read #%d (cset=%s): '%s'\n",
               test, (cset == H5T_CSET_ASCII ? "ASCII" : "UTF8"), rdata[0]);

        // Close and release resources
        space = H5Dget_space(dset);
        H5Dvlen_reclaim(memtype, space, H5P_DEFAULT, rdata);
        H5Sclose(space);
        H5Tclose(memtype);
        H5Dclose(dset);
    }

    H5Fclose(file);
}

int main(int argc, char **argv)
{
    // Create the file and store the string
    write_dataset();

    // Read the string from the file
    read_dataset(argc > 1);

    return 0;
}
