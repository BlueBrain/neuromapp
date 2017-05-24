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
    void block_compress(prac_block *full_block) {
        //process create compression buffer, and use zlib compress utitilty
        uLong comp_sourceLen = sizeof(full_block);
        uLong comp_destLen = compressBound(comp_sourceLen);
        void * temp =  malloc(sizeof(unsigned int)*comp_destLen);// what should type be here?
        Bytef * dest_ptr =(Bytef*) temp; 
        // might need reinterpret_cast, still seems like maybe this is outside the scope for vanilla compress utility
        const Bytef* source_ptr =(Bytef*)&full_block;
        std::cout <<  " attempting compression " << std::endl;
        int rc = compress(dest_ptr,&comp_destLen,source_ptr,comp_sourceLen);
        std::cout <<  "the rc was " <<rc << std::endl;
        if (rc == Z_OK) {
            std::cout << " size of compressed block is "<< comp_destLen << " as compared with " << comp_sourceLen << std::endl;
        }
        // try to  uncompress the temp
        //so source lens and stuff are the destLen from before
        //make a new block of same dimensions
        int rows = full_block->num_rows();
        int cols = full_block->num_cols();
        prac_block uncomp_block(cols,rows);
        uncomp_block.print();
        //we use the previous dest_len as our source
        uLong uncomp_sourceLen = comp_destLen;
        uLong uncomp_destLen = compressBound(uncomp_sourceLen); 
        uncompress((Bytef*) &uncomp_block,&uncomp_destLen,(Bytef*) temp,uncomp_sourceLen);
        uncomp_block.print();
    }


}
