#ifndef H5BMARK_H5UTIL
#define H5BMARK_H5UTIL

#include <string>
#include <vector>

#define NAME_LENGTH_MAX 256

namespace h5benchmark
{
    typedef char fixedstring_t[NAME_LENGTH_MAX];
    
    class H5Util
    {
        public:
            /**
             * Converts a vector<string> to a buffer of fixed-length strings.
             */
            static fixedstring_t* vectorConv(std::vector<std::string> strings);
            
            /**
             * Converts a buffer of fixed-length strings to a vector<string>.
             */
            static std::vector<std::string> bufferConv(fixedstring_t *strings, 
                                                       size_t count);
        
        private:
            H5Util() { }
            ~H5Util() { }
    };
}

#endif
