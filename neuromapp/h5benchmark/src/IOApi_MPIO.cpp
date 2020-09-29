
#include "IOApi_MPIO.hpp"

using namespace h5benchmark;

#define BUFFER_SIZE (1048576 * 256) // TODO: Check for dataset sizes!

IOApiMPIO::IOApiMPIO(std::string filename, bool enable_phdf5)
{
    MPI_File_open(MPI_COMM_SELF, filename.c_str(), MPI_MODE_RDONLY, 
                  MPI_INFO_NULL, &m_file);
    m_buffer = malloc(BUFFER_SIZE);
}

IOApiMPIO::~IOApiMPIO()
{
    MPI_File_close(&m_file);
    free(m_buffer);
}

int IOApiMPIO::readGroup(group_t &group)
{
    auto &dataset = group.dataset;
    
    // Read the content available on each dataset
    MPI_File_read_at(m_file, dataset[0].offset, m_buffer, dataset[0].size,
                     MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_read_at(m_file, dataset[1].offset, m_buffer, dataset[1].size,
                     MPI_DOUBLE, MPI_STATUS_IGNORE);
    
    // <<<<<<<<<<<<<<<<<<<<< Type conversion from double to float is missing!!
    
    // printf ("Dataset: %s\n    (%lf,%lf,%lf,%lf)\n", group.name,
    //                                                 ((double *)m_buffer)[0],
    //                                                 ((double *)m_buffer)[1],
    //                                                 ((double *)m_buffer)[2],
    //                                                 ((double *)m_buffer)[3]);
    
    return 0;
}
