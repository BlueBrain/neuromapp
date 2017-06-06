/*this is the compression policy file to be used by the block*/
#ifndef neuromapp_COMPRESSOR
#define neuromapp_COMPRESSOR

#include <cstdlib>
#include <stdexcept> 
#include <cstdlib>
#include "zlib.h"
#include "block.h"
//TODO change back from std namespace 
using namespace std;
namespace neuromapp {
    class no_compress {
        public:
            void compress_policy(void * data_source, size_t uncompressed_size ){ 
                std::cout << "compression library not found, can't compress, leaving the same size as before" << std::endl;
            }

            void uncompress_policy(void * data_source,size_t compressed_size,size_t uncompressed_size) {
                std::cout << "no compression library found" << std::endl;
            }
    };

    class zlib {
        public:
            //tim's wisdom, don't ppass block as argument, just pointer to data and size (pure C style)
            void compress_policy(void * data_source, size_t uncompressed_size) {
                //get the approximate size in memory of the data_source
                uLong source_len = (uLong) uncompressed_size;
                uLong dest_len = compressBound(source_len);
                //create void buffer holder dest for the compressed block
                void * dest = malloc(dest_len);// creates enough space for the compressed block
                // assign Bytef* for source and dest
                Bytef* source_ptr = (Bytef*) data_source;// hopefully typedef pointer (data() return) can be coerced to Bytef*
                Bytef* dest_ptr = (Bytef*) dest;
                //perform the actual compression
                int rc = compress(dest_ptr,&dest_len,source_ptr,source_len);
                //check the rc to evaluate compress success
                switch (rc) {
                    case Z_OK:
                        std::cout << "compress worked" << std::endl;
                        break;
                    case Z_BUF_ERROR:
                        std::cerr << "ran out of space in compress buffer"<< std::endl;
                        throw runtime_error("");
                        break;
                    case Z_MEM_ERROR:
                        std::cerr << "ran out of memory during compression"<< std::endl;
                        throw runtime_error("");
                        // do we still need the break underneath this?
                        break;
                }
                //swap teh memory so taht we have change the data_source in place
                std::swap(dest,data_source);
            }


            void uncompress_policy(void * data_source, size_t compressed_size, size_t uncompressed_size) {
                //original amount of memory used is still discernable
                uLong dest_len = (uLong) uncompressed_size;
                void * dest = std::malloc(dest_len);
                //need to get the compressed source size to work with
                uLong source_len = compressed_size;
                //set pointers for uncompress
                Bytef* source_ptr = (Bytef*) data_source;
                Bytef* dest_ptr = (Bytef*) dest;
                //perform the uncompress
                uncompress(dest_ptr,&dest_len,source_ptr,source_len);
                //swap the data
                std::swap(data_source,dest);
            }
    };

    //end of neuromapp namespace
}

#endif



