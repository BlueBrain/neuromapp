
#include <H5Opublic.h>
#include <H5Dpublic.h>
#include <mpi.h>
#include "H5Parser.hpp"

using namespace h5benchmark;

#define INDEX_SUFFIX ".idx"

H5Parser::H5Parser(std::string filename, bool enable_phdf5)
    : m_file(-1)
    , m_file_idx(MPI_FILE_NULL)
    , m_filename_idx(filename + INDEX_SUFFIX)
{
    // Try to open the cached index first
    MPI_File_open(MPI_COMM_SELF, m_filename_idx.c_str(),
                  MPI_MODE_RDONLY, MPI_INFO_NULL, &m_file_idx);

    if (m_file_idx == MPI_FILE_NULL)
    {
        hid_t fapl_id = H5P_DEFAULT;
        
        // Enable the MPI-IO driver for pHDF5, if needed
        if (enable_phdf5)
        {
            fapl_id = H5Pcreate(H5P_FILE_ACCESS);
            H5Pset_fapl_mpio(fapl_id, MPI_COMM_SELF, MPI_INFO_NULL);
        }
        
        m_file = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, fapl_id);
    }
}

H5Parser::~H5Parser()
{
    if (m_file >= 0)
    {
        H5Fclose(m_file);
    }
    else
    {
        MPI_File_close(&m_file_idx);
    }
}

/**
 * Operator function to search for all the groups using H5Literate.
 */
herr_t op_func(hid_t loc_id, const char *name, const H5L_info_t *info,
               void *op_data)
{
    auto       groups  = static_cast<std::vector<group_t> *>(op_data);
    H5O_info_t infobuf = { 0 };

    // Get type of the object with the provided name
    if (!H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT) &&
        (infobuf.type == H5O_TYPE_GROUP || infobuf.type == H5O_TYPE_DATASET))
    {
        if (infobuf.type == H5O_TYPE_GROUP)
        {
            groups->push_back(group_t(name));
            
            // printf("Group: %s (addr=%zu)\n", name, infobuf.addr);
            
            return H5Literate_by_name(loc_id, name, H5_INDEX_NAME, H5_ITER_NATIVE,
                                    NULL, op_func, op_data, H5P_DEFAULT);
        }
        
        auto dataset    = &groups->back().dataset[!strcmp("points", name)];
        auto did        = H5Dopen(loc_id, name, H5P_DEFAULT);
        auto did_type   = H5Dget_type(did); // <<<<<<<<<<<<<<<<<<< Not working!
        
        dataset->offset = (off_t)H5Dget_offset(did);
        dataset->size   = (size_t)H5Dget_storage_size(did);
        dataset->type   = (did_type == H5T_INTEGER) ? TYPE_INT   :
                          (did_type == H5T_FLOAT)   ? TYPE_FLOAT :
                                                      TYPE_DOUBLE;
        
        H5Tclose(did_type);
        H5Dclose(did);
        
        // printf("    > Dataset: %s (offset=%zu size=%zu type=%d)\n",
        //                 name, dataset->offset, dataset->size, dataset->type);
        
    }
    
    return 0;
}

int H5Parser::getGroups(std::vector<group_t> &groups)
{
    int hr = 0;

    // Get the group information from the index, if possible
    if (m_file_idx != MPI_FILE_NULL)
    {
        MPI_Offset size = 0;
        MPI_File_get_size(m_file_idx, &size);

        groups.resize(size / sizeof(group_t));

        // Read the content available on each dataset
        MPI_File_read(m_file_idx, &groups[0], size, MPI_BYTE,
                      MPI_STATUS_IGNORE);
    }
    else
    {
        hr = H5Literate(m_file, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, op_func,
                        static_cast<void *>(&groups));
        
        // Cache the index for future runs
        if (hr == 0)
        {
            MPI_Offset size = groups.size() * sizeof(group_t);

            MPI_File_open(MPI_COMM_SELF, m_filename_idx.c_str(),
                          (MPI_MODE_CREATE | MPI_MODE_RDWR), MPI_INFO_NULL,
                          &m_file_idx);
            MPI_File_write(m_file_idx, &groups[0], size, MPI_BYTE,
                           MPI_STATUS_IGNORE);
            
            // Close the HDF5 file
            H5Fclose(m_file);
            m_file = -1;
        }
    }

    return hr;
}
