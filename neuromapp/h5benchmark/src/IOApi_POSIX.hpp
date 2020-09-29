#ifndef H5BMARK_IOAPI_POSIX
#define H5BMARK_IOAPI_POSIX

#include "IOApi.hpp"

namespace h5benchmark
{
    class IOApiPOSIX : public IOApi
    {
        public:
            IOApiPOSIX(std::string filename, bool enable_mpiio);
            virtual ~IOApiPOSIX();
            
            virtual int readGroup(group_t &group);
            
        private:
            int  m_file;    // File handle
            void *m_buffer; // Buffer to store the datasets
    };
}

#endif
