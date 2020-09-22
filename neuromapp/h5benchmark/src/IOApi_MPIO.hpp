#ifndef H5BMARK_IOAPI_MPIO
#define H5BMARK_IOAPI_MPIO

#include <mpi.h>
#include "IOApi.hpp"

namespace h5benchmark
{
    class IOApiMPIO : public IOApi
    {
        public:
            IOApiMPIO(std::string filename, bool enable_mpiio);
            virtual ~IOApiMPIO();
            
            virtual int readGroup(group_t &group);
            
        private:
            MPI_File m_file;    // File handle
            void     *m_buffer; // Buffer to store the datasets
    };
}

#endif
