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
            unsigned int sign:1;
            unsigned int exponent:8;
            unsigned int mantissa:23;
        } parts;
    } type_parts;


    /*function for taking in an array of floats ( TODO make generic) 
     * and creating an unsigned int array that represents its parts
     */
    void convert_to_parts(pointer row_ptr,block<unsigned int,cstandard> & parts_block,size_type row_num){
        //TODO make sure to free after use!
        size_type count = parts_block.dim0()/3;
        for (size_type i = 0 ; i < count ; i++) {
            type_parts conv_float;
            conv_float.num = *row_ptr++;
            parts_block(i,row_num) = conv_float.parts.sign;
            parts_block(i+count,row_num) = conv_float.parts.exponent;
            parts_block(i+count*2,row_num) = conv_float.parts.mantissa;
        }
    }

    void populate_block_by_parts( block<unsigned int,cstandard> & parts_block,block<value_type,cstandard>& float_block) {
        size_type row_limit = float_block.num_rows();
        for (size_type row_num = 0; row_num < row_limit;row_num++) {
            convert_to_parts(&float_block(0,row_num),parts_block,row_num);
        }
    }
}

