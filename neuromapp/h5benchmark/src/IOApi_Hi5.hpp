#ifndef H5BMARK_IOAPI_HI5
#define H5BMARK_IOAPI_HI5

#include "highfive/H5File.hpp"
#include "IOApi.hpp"

namespace h5benchmark
{
    class IOApiH5 : public IOApi
    {
        public:
            IOApiH5(std::string filename, bool enable_mpiio, bool enable_boost);
            virtual ~IOApiH5();
            
            virtual int readGroup(std::string name);
            
        private:
            HighFive::File m_file;          // File handle
            const bool     m_boost_enabled; // Flag to use Boost
    };
}

#endif
