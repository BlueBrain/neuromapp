#ifndef H5BMARK_IOAPI
#define H5BMARK_IOAPI

#include <string>

namespace h5benchmark
{
    /**
     * Class that defines the necessary methods required to implement an I/O
     * API for the H5Benchmark. 
     */
    class IOApi
    {
        public:
            /**
             * Reads the structure and points datasets from a given group name. 
             */
            virtual int readGroup(std::string name) = 0;
    };
}

#endif
