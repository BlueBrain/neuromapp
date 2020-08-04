#ifndef H5BMARK_IOAPI_MKIT
#define H5BMARK_IOAPI_MKIT

#include "morphokit/storage.h"
#include "IOApi.hpp"

namespace h5benchmark
{
    class IOApiMKit : public IOApi
    {
        public:
            IOApiMKit(std::string filename, bool enable_mpiio);
            virtual ~IOApiMKit();
            
            virtual int readGroup(std::string name);
            
        private:
            typedef std::shared_ptr<morphokit::FileStorage> FStorage;
            
            FStorage m_file;    // File handle
    };
}

#endif
