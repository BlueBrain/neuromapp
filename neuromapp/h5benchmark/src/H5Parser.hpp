#ifndef H5BMARK_H5PARSER
#define H5BMARK_H5PARSER

#include <string>
#include <vector>
#include <hdf5.h>
#include "IOApi.hpp"

namespace h5benchmark
{
    class H5Parser
    {
        public:
            H5Parser(std::string filename, bool enable_mpiio);
            ~H5Parser();
            
            int getGroups(std::vector<group_t> &groups);
            
        private:
            hid_t       m_file;         // File handle
            MPI_File    m_file_idx;     // File handle for the index
            std::string m_filename_idx; // Filename for the index
    };
}

#endif
