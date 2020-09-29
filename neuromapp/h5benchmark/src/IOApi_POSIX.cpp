
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "IOApi_POSIX.hpp"

using namespace h5benchmark;

#define BUFFER_SIZE (1048576 * 256) // TODO: Check for dataset sizes!

IOApiPOSIX::IOApiPOSIX(std::string filename, bool enable_phdf5)
{
    m_file   = open(filename.c_str(), O_RDONLY);
    m_buffer = malloc(BUFFER_SIZE);
}

IOApiPOSIX::~IOApiPOSIX()
{
    close(m_file);
    free(m_buffer);
}

int IOApiPOSIX::readGroup(group_t &group)
{
    auto &dataset = group.dataset;
    
    // Read the content available on each dataset
    pread(m_file, m_buffer, dataset[0].size, dataset[0].offset);
    pread(m_file, m_buffer, dataset[1].size, dataset[1].offset);
    
    // <<<<<<<<<<<<<<<<<<<<< Type conversion from double to float is missing!!
    
    // printf ("Dataset: %s\n    (%lf,%lf,%lf,%lf)\n", group.name,
    //                                                 ((double *)m_buffer)[0],
    //                                                 ((double *)m_buffer)[1],
    //                                                 ((double *)m_buffer)[2],
    //                                                 ((double *)m_buffer)[3]);
    
    return 0;
}
