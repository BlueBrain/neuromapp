#include <iostream>
#include <string>
#include <algorithm>
#include "block.h"
//TODO remove std namespace

using namespace std;
using neuromapp::block;
typedef size_t size_type;
typedef float value_type;
typedef float* pointer;

namespace neuromapp {
    typedef union {
        float num;
        struct {
            unsigned int mantissa:23;
            unsigned int exponent:8;
            unsigned int sign:1;
        } parts;
    } conversion_parts;


    /*function for taking in an array of floats ( TODO make generic) 
     * and creating an unsigned int array that represents its parts
     */
    void convert_to_parts(pointer row_ptr,block<unsigned int,cstandard> & split_block,size_type row_num){
        size_type count = split_block.dim0()/3;
        for (size_type i = 0 ; i < count ; i++) {
            conversion_parts conv_float;
            conv_float.num = *row_ptr++;
            split_block(i,row_num) = conv_float.parts.sign;
            split_block(i+count,row_num) = conv_float.parts.exponent;
            split_block(i+count*2,row_num) = conv_float.parts.mantissa;
        }
    }

    /*reciprocal function to teh one above, take values from the split block, and convert them back to their floating point representation
     * and store them back in the unsplit block row pointer
     */
    void convert_from_parts(pointer row_ptr,block<unsigned int,cstandard> & split_block,size_type row_num){
        size_type count = split_block.dim0()/3;
        for (size_type i = 0 ; i < count ; i++) {
            conversion_parts conv_float;
            conv_float.parts.sign=split_block(i,row_num) ;
            conv_float.parts.exponent=split_block(i+count,row_num) ;
            conv_float.parts.mantissa=split_block(i+count*2,row_num) ;
            *row_ptr++ = conv_float.num;
        }
    }

    void populate_split_block( block<unsigned int,cstandard> & split_block,block<value_type,cstandard>& unsplit_block) {
        size_type row_limit = unsplit_block.num_rows();
        for (size_type row_num = 0; row_num < row_limit;row_num++) {
            convert_to_parts(&unsplit_block(0,row_num),split_block,row_num);
        }
    }

    void populate_unsplit_block( block<unsigned int,cstandard> & split_block,block<value_type,cstandard>& unsplit_block) {
        size_type row_limit = unsplit_block.num_rows();
        for (size_type row_num = 0; row_num < row_limit;row_num++) {
            convert_from_parts(&unsplit_block(0,row_num),split_block,row_num);
        }
    }


    block<unsigned int, cstandard> generate_split_block(block<value_type,cstandard> & unsplit_block) {
        size_type rows_ = unsplit_block.num_rows(),cols_=unsplit_block.dim0();
        block<unsigned int,cstandard> split_block(cols_,rows_);
        populate_split_block(split_block,unsplit_block);
        return split_block;
    }
    
    block<value_type,cstandard> generate_unsplit_block(block<unsigned int,cstandard> & split_block) {
        size_type rows_ = split_block.num_rows(),cols_=split_block.dim0();
        block<value_type,cstandard> unsplit_block(cols_/3,rows_);
        populate_unsplit_block(split_block,unsplit_block);
        return unsplit_block;
    }
}

