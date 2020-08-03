#ifndef H5BMARK_IOAPI_HDF5
#define H5BMARK_IOAPI_HDF5

#include <hdf5.h>
#include "IOApi.hpp"

namespace h5benchmark
{
    class IOApiHDF5 : public IOApi
    {
        public:
            IOApiHDF5(std::string filename, bool enable_mpiio);
            virtual ~IOApiHDF5();
            
            virtual int readGroup(std::string name);
            
        private:
            hid_t m_file;    // File handle
            hid_t m_plist;   // Transfer property list
            void  *m_buffer; // Buffer to store the datasets
    };
}

#endif
