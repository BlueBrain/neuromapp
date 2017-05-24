#include <stdexcept> 
#include <cstdlib>
#include "compression/compression.h"
#include "compression/block.h"
#include "/usr/include/zlib.h"

using namespace neuromapp;
using prac_block = block<int,cstandard>;
// include the zlib header.
namespace zlib {
    void hello_zlib() {
        char a[] = "hello world";
        char b[50];
        z_stream def_stream;// this means the struct that will be used for deflation

        //and now setting the initial struct variables
        def_stream.zalloc = Z_NULL;
        def_stream.zfree = Z_NULL;
        def_stream.opaque = Z_NULL;
        def_stream.avail_in = (uInt)strlen(a) + 1;// makes space for the null char?
        def_stream.next_in = (Bytef*) a;// this means the first byte is from array a
        def_stream.next_out = (Bytef*)b;
        deflateInit(&def_stream,Z_DEFAULT_COMPRESSION);// initializes the compression of array a
        deflate(&def_stream,Z_FINISH);// begins compression
        deflateEnd(&def_stream);// frees the structure used for compression

        // method for getting size of compressed array
        printf("Deflated size is :%lu\n",(char*)def_stream.next_out -b );// think about this line
        /* vector<char> file_chars(start,end); */
        /* for_each(file_chars.begin(),file_chars.end(),[](char letter){cout << ((letter == 'e') ? letter : 'X');}); */
    }

    // todo make general to any allocator choice, cstandard and the align
    void block_compress() {
        //process create compression buffer, and use zlib compress utitilty
        char orig_str[] = "compress this";
        uLong sourceLen = strlen(orig_str) + 1;
        uLong destLen = compressBound(sourceLen);
        void * temp =  malloc(sizeof(unsigned char)*destLen);
        Bytef * dest_ptr =(Bytef*) temp; 
        const Bytef* source_ptr =(Bytef*) orig_str;
        std::cout <<  " attempting compression " << std::endl;
        int rc = compress(dest_ptr,&destLen,source_ptr,sourceLen);
        std::cout <<  "the rc was " <<rc << std::endl;
        



    }


}
