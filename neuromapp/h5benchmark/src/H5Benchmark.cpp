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
#include <hdf5.h>
#include "highfive/H5File.hpp"

using namespace std;
using namespace HighFive;

#define BUFFER_SIZE (1048576 * 256) // TODO: Check for dataset sizes!

int main(int argc, char **argv)
{
    // char *buffer = (char*)malloc(BUFFER_SIZE);
    // auto file = H5Fopen("/gpfs/bbp.cscs.ch/project/proj12/jenkins/cellular/circuit-1k/nodes.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
    // for (int i = 1; i < argc; i++)
    // {
    //     hid_t did = H5Dopen2(file, argv[i], H5P_DEFAULT);
    //     auto datatype_id = H5Tcopy(H5T_C_S1);
    //     H5Tset_size(datatype_id, H5T_VARIABLE);
    //     H5Tset_cset(datatype_id, H5T_CSET_UTF8);
    //     // printf("UTF8=%d ASCII=%d provided=%d\n", H5T_CSET_UTF8, H5T_CSET_ASCII, H5Tget_cset(H5Dget_type(did)));
    //     if (H5Dread(did, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer) < 0)
    //     {
    //         printf("Error atopeeeeeeeeee!!\n");
    //         return -1;
    //     }
    //     printf("Atopeeeeeeeeeeeee!!\n");
    // }

    // vector<std::string> buffer;
    // File file("/gpfs/bbp.cscs.ch/project/proj12/jenkins/cellular/circuit-1k/nodes.h5");
    // for (int i = 1; i < argc; i++)
    //     file.getDataSet(std::string(argv[i])).read(buffer);
    
    // File file("./test.h5", File::ReadWrite | File::Create | File::Truncate);
    // std::string buffer = "Hello!";
    // DataSet dataset = file.createDataSet<std::string>("/testdataset",  DataSpace::From(buffer));
    // dataset.write(buffer);

    // std::string buffer;
    // File file("./test.h5");
    // file.getDataSet("/testdataset").read(buffer);

    char *buffer = (char*)malloc(BUFFER_SIZE);
    auto file = H5Fopen("./test.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
    for (int i = 0; i < 2; i++)
    {
        hid_t did = H5Dopen2(file, "/testdataset", H5P_DEFAULT);
        auto datatype_id = H5Tcopy(H5T_C_S1);
        H5Tset_size(datatype_id, H5T_VARIABLE);
        H5Tset_cset(datatype_id, (i==0) ? H5T_CSET_UTF8 : H5T_CSET_ASCII);
        if (H5Dread(did, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer) < 0)
        {
            printf("Error atopeeeeeeeeee!!\n");
            return -1;
        }
        std::cout << std::string(buffer) << std::endl;
    }

    return 0;
}
