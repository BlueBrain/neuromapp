
#include <mpi.h>
#include "IOApi_HDF5.hpp"

using namespace h5benchmark;

#define BUFFER_SIZE (1048576 * 256) // TODO: Check for dataset sizes!

IOApiHDF5::IOApiHDF5(std::string filename, bool enable_phdf5) :
    m_file(0),
    m_plist(H5P_DEFAULT)
{
    hid_t fapl_id = H5P_DEFAULT;
    
    // Enable the MPI-IO driver for pHDF5, if needed
    if (enable_phdf5)
    {
        fapl_id = H5Pcreate(H5P_FILE_ACCESS);
        H5Pset_fapl_mpio(fapl_id, MPI_COMM_WORLD, MPI_INFO_NULL);
        
        m_plist = H5Pcreate(H5P_DATASET_XFER);
        H5Pset_dxpl_mpio(m_plist, H5FD_MPIO_INDEPENDENT);
        // Alternative: H5FD_MPIO_COLLECTIVE
    }
    
    m_file   = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, fapl_id);
    m_buffer = malloc(BUFFER_SIZE);
}

IOApiHDF5::~IOApiHDF5()
{
    H5Fclose(m_file);
    free(m_buffer);
}

int IOApiHDF5::readGroup(group_t &group)
{
    hid_t gid    = 0;
    hid_t did[2] = { 0 };
    
    // Retrieve the group and open the datasets
    gid    = H5Gopen2(m_file, group.name,  H5P_DEFAULT);
    did[0] = H5Dopen2(gid,    "structure", H5P_DEFAULT);
    did[1] = H5Dopen2(gid,    "points",    H5P_DEFAULT);
    
    // Read the content available on each dataset
    H5Dread(did[0], H5T_NATIVE_INT,    H5S_ALL, H5S_ALL, m_plist, m_buffer);
    H5Dread(did[1], H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, m_plist, m_buffer);
    
    // <<<<<<<<<<<<<<<<<<<<< Type conversion from double to float is missing!!
    
    // printf ("Dataset: %s\n    (%lf,%lf,%lf,%lf)\n", group.name,
    //                                                 ((double *)m_buffer)[0],
    //                                                 ((double *)m_buffer)[1],
    //                                                 ((double *)m_buffer)[2],
    //                                                 ((double *)m_buffer)[3]);
    
    H5Dclose(did[0]);
    H5Dclose(did[1]);
    H5Gclose(gid);
    
    return 0;
}
