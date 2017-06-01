/*this is the compression policy file to be used by the block*/
#ifndef neuromapp_COMPRESSOR
#define neuromapp_COMPRESSOR

#include <cstdlib>
#include <stdexcept> 
#include <cstdlib>
#include "compression/compression.h"
#include "compression/allocator.h"
#include "compression/block.h"
#include "/usr/include/zlib.h"

//TODO change back from std namespace 
using namespace std;
namespace neuromapp {

    class zlib {
        public:
            template<typename T>
                void compress_policy(block<T,cstandard> block_norm) {
                    //get the approximate size in memory of the block_norm

                    uLong source_len = block_norm.memory_allocated();
                    uLong dest_len = compressBound(source_len);
                    //create void buffer holder dest for the compressed block
                    void * dest = malloc(dest_len);// creates enough space for the compressed block
                    // assign Bytef* for source and dest
                    Bytef* source_ptr = (Bytef*) block_norm.data();// hopefully typedef pointer (data() return) can be coerced to Bytef*
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
                    //change the block data with the compressed form
                    std::swap(dest,block_norm.data());
                }

            template<typename T>
            void uncompress_policy(block<t,cstandard> compressed_blk) {
                //original amount of memory used is still discernable
                uLong dest_len = compressed_blk.memory_allocated();
                void * dest = std::malloc(destLen);
                //need to get the compressed source size to work with
                uLong source_len = compressed_blk.size();
                //set pointers for uncompress
                Bytef* source_ptr = (Bytef*) compressed_blk.data();
                Bytef* dest_ptr = (Bytef*) dest;
                //perform the uncompress
                uncompress(dest_ptr,&dest_len,source_ptr,source_len);
                //swap the data
                std::swap(compressed_blk.data(),dest);
            }
    };

    //end of neuromapp namespace
}




