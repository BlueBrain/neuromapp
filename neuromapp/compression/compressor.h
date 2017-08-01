/* Filename : compressor.h
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, Blue Brain Project
 * Purpose : xxx
 * Date : 2017-08-01 
 */
/* Filename : compressor.h
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, EPFL
 * Purpose : xxx
 * Date : 2017-07-20 
 */
/*this is the compression policy file to be used by the block*/
#ifndef neuromapp_COMPRESSOR
#define neuromapp_COMPRESSOR

#include <cstdlib>
#include <stdexcept> 
#include <cstdlib>
#include "zlib.h"
#include "compress_exception.h"
#include "block.h"
//TODO change back from std namespace 
using namespace std;
typedef size_t size_type;
namespace neuromapp {

    class no_compress {
        public:
        /**
        * compress_policy 
        *
        *
        * @brief
        *
        * @param void * data_source, size_type uncompressed_size 
        *
        * @return void
        */
            void compress_policy(void * data_source, size_type uncompressed_size ){ 
                std::cout << "compression library not found, can't compress, leaving the same size as before" << std::endl;
            }

        /**
        * uncompress_policy 
        *
        *
        * @brief
        *
        * @param void * data_source,size_type compressed_size,size_type uncompressed_size
        *
        * @return void
        */
            void uncompress_policy(void * data_source,size_type compressed_size,size_type uncompressed_size) {
                std::cout << "no compression library found" << std::endl;
            }
    };

        class zlib {
            public:
                //double pointer so we can change the upper_level within the function
                // helper function for checking compression return codes
        /**
        * check_compression_rc 
        *
        *
        * @brief
        *
        * @param int rc
        *
        * @return void
        */
                void check_compression_rc (int rc) {
                    switch (rc) {
                        case Z_OK:
                            break;
                        case Z_BUF_ERROR:
                            throw zlib_exception("no more buffer space for compression/uncompression",Z_BUF_ERROR);
                            break;
                        case Z_MEM_ERROR:
                            throw zlib_exception("ran out of memory for zlib operation",Z_MEM_ERROR);
                            // do we still need the break underneath this?
                            break;
                    }
                }

                template<typename value_type>
        /**
        * compress_policy 
        *
        *
        * @brief
        *
        * @param value_type ** data_source, size_type *uncompressed_size
        *
        * @return void
        */
                void compress_policy(value_type ** data_source, size_type *uncompressed_size) {
                    //get the approximate size in memory of the data_source
                    uLong source_len = (uLong) *uncompressed_size;
                    uLong dest_len = compressBound(source_len);
                    //create void buffer holder dest for the compressed block
                    value_type * dest = (value_type *) malloc(dest_len);// creates enough space for the compressed block
                    // assign Bytef* for source and dest
                    Bytef* source_ptr = (Bytef*) *data_source;// hopefully typedef pointer (data() return) can be coerced to Bytef*
                    Bytef* dest_ptr = (Bytef*) dest;
                    //perform the actual compression
                    int rc = compress(dest_ptr,&dest_len,source_ptr,source_len);
                    check_compression_rc(rc);
                    //check the rc to evaluate compress success
                    //update the size of the block
                    *uncompressed_size = dest_len;
                    // change the uncompressed data out for compressed
                    swap(*data_source,dest);
                    free (dest);
                }


                //double pointer so we can change the upper_level within the function
                template<typename value_type>
        /**
        * uncompress_policy 
        *
        *
        * @brief
        *
        * @param value_type ** data_source, size_type *compressed_size, size_type uncompressed_size
        *
        * @return void
        */
                void uncompress_policy(value_type ** data_source, size_type *compressed_size, size_type uncompressed_size) {
                    //original amount of memory used is still discernable
                    uLong dest_len = (uLong) uncompressed_size;
                    value_type * dest = (value_type *) std::malloc(dest_len);
                    //need to get the compressed source size to work with
                    uLong source_len = *compressed_size;
                    //set pointers for uncompress
                    Bytef* source_ptr = (Bytef*) *data_source;
                    Bytef* dest_ptr = (Bytef*) dest;
                    //perform the uncompress
                    int rc = uncompress(dest_ptr,&dest_len,source_ptr,source_len);
                    check_compression_rc(rc);
                    //swap the data
                    *compressed_size = uncompressed_size;
                    swap(*data_source,dest);
                    free(dest);
                }

        };

    //end of neuromapp namespace
}

#endif



