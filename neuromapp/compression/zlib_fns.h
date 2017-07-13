#include <stdexcept> 
#include <vector>
#include <cstdlib>
#include "compression/compression.h"
#include "compression/block.h"
#include <zlib.h>

using namespace neuromapp;
using prac_block = block<int,cstandard>;
// include the zlib header.
//TODO change back to non std namespace, currently for experimentation
using namespace std;
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
    void check_compress_res(int comp_rc ) {
        switch(comp_rc) {
            case Z_OK:
                std::cout << "compression passed got Z_OK" << std::endl;
                break;

            case Z_BUF_ERROR:
                std::cerr << "not enough space in the buffer" << std::endl;
                throw  std::runtime_error("");
                break;
            case Z_MEM_ERROR:
                std::cerr << "ran out of memory" << std::endl;
                throw  std::runtime_error("");
                break;
        }
    }

    // todo make general to any allocator choice, cstandard and the align
    void block_compress() {
        //process create compression buffer, and use zlib compress utitilty
        //trying to zip and unzip on regular vector
        vector<int> source {1,2,3,4};
        //need to get the full size of the vector, so 
        uLong source_len = sizeof(source) + sizeof(int)*source.capacity();
        // make a non-specific block of memory
        std::cout << "sourcelen is " << source_len << std::endl;
        uLong dest_len = compressBound(source_len);
        std::cout << "destlen is " << dest_len << std::endl;
        void * dest = malloc(dest_len);
        // try using address of first byte for each 
        Bytef* source_ptr = (Bytef*) &source[0];
        Bytef* dest_ptr = (Bytef*) dest;
        int comp_rc = compress(dest_ptr,&dest_len,source_ptr,source_len);
        check_compress_res(comp_rc);
        //if we make it here now we try to uncompress
        //why does the destlen change from the beginning?
        std::cout << "destlen has become" << dest_len << std::endl;
        vector<int> uncomp(10);// make capacity 4
        uLong uncomp_len = compressBound(dest_len);
        Bytef* uncomp_ptr = (Bytef*) &uncomp[0];
        comp_rc =uncompress(uncomp_ptr,&uncomp_len,dest_ptr,dest_len);
        check_compress_res(comp_rc);
        //make it this far we need to print out the results of the compression
        vector<int>::iterator it = uncomp.begin();
        for (;it != uncomp.end();it++) {
            std::cout << *it <<", " << std::endl;
        }


    }


}
