
#include <mpi.h>
#include "H5Parser.hpp"

using namespace h5benchmark;

H5Parser::H5Parser(std::string filename, bool enable_phdf5)
{
    hid_t fapl_id = H5P_DEFAULT;
    
    // Enable the MPI-IO driver for pHDF5, if needed
    if (enable_phdf5)
    {
        fapl_id = H5Pcreate(H5P_FILE_ACCESS);
        H5Pset_fapl_mpio(fapl_id, MPI_COMM_WORLD, MPI_INFO_NULL);
    }
    
    m_file = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, fapl_id);
}

H5Parser::~H5Parser()
{
    H5Fclose(m_file);
}

/**
 * Operator function to search for all the groups using H5Literate.
 */
herr_t op_func(hid_t loc_id, const char *name, const H5L_info_t *info,
               void *op_data)
{
    auto       groups  = static_cast<std::vector<std::string> *>(op_data);
    H5O_info_t infobuf = { 0 };

    // Get type of the object with the provided name
    if (!H5Oget_info_by_name(loc_id, name, &infobuf, H5P_DEFAULT) &&
        infobuf.type == H5O_TYPE_GROUP)
    {
        groups->push_back(std::string(name));
        
        return H5Literate_by_name(loc_id, name, H5_INDEX_NAME, H5_ITER_NATIVE,
                                  NULL, op_func, op_data, H5P_DEFAULT);
    }
    
    return 0;
}

int H5Parser::getGroups(std::vector<std::string> &groups)
{
    return H5Literate(m_file, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, op_func,
                      static_cast<void *>(&groups));
}
