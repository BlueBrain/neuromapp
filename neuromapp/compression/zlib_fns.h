#include <stdexcept> 
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
    void block_compress(prac_block * full_block) {
        //process create compression buffer, and use zlib compress utitilty        
        int comp_RC;
        uLong  sourceLen = (uLong) sizeof(*full_block);
        uLongf * destLen = (uLongf*) compressBound(sourceLen);
        if ( sourceLen > *destLen ) {
            std::cerr << "destLen is " << destLen << " compared with " << sourceLen << std::endl;
            throw std::runtime_error ("problem with dest len");
        }
        void * raw_compressed_buf = malloc(*destLen);
        //the Bytef*'s are pointers to the first position in both rawComp and full_block
        comp_RC = compress((Bytef*)raw_compressed_buf, destLen, (Bytef*)full_block, sourceLen);
        switch (comp_RC) {
            case Z_OK :
                std::cout <<" worked! compressed size is "<< destLen << " vs " << sourceLen << std::endl;
                break;
            case Z_MEM_ERROR :
                std::cout << "mem error, not enough memory probably not big enough" << std::endl;
                break;
            case Z_BUF_ERROR:
                std::cout << "buff errro not enough space in thebuffer" << std::endl;
                break;
        }




    }


}
