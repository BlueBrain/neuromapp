#ifndef H5BMARK_H5PARSER
#define H5BMARK_H5PARSER

#include <string>
#include <vector>
#include <hdf5.h>

namespace h5benchmark
{
    class H5Parser
    {
        public:
            H5Parser(std::string filename, bool enable_mpiio);
            ~H5Parser();
            
            int getGroups(std::vector<std::string> &groups);
            
        private:
            hid_t m_file;   // File handle
    };
}

#endif
