#ifndef H5BMARK_IOAPI
#define H5BMARK_IOAPI

#include <string>
#include <cstring>

namespace h5benchmark
{
    typedef enum
    {
        TYPE_INT    = 0,
        TYPE_FLOAT  = 1,
        TYPE_DOUBLE = 2,
    } type_t;
    
    typedef struct _dataset_t
    {
        off_t  offset;
        off_t  size;
        type_t type;
    } dataset_t;
    
    typedef struct _group_t
    {
        char name[256];
        dataset_t dataset[2];
        
        _group_t()
        {
            memset(name,    0, sizeof(name));
            memset(dataset, 0, sizeof(dataset));
        }
        
        _group_t(const char *_name) : _group_t()
        {
            strncpy(name, _name, sizeof(name));
        }
    } group_t;
    
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
            virtual int readGroup(group_t &group) = 0;
            
            virtual ~IOApi() { }
    };
}

#endif
