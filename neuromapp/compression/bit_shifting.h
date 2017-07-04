#include <iostream>
#include <string>
#include <algorithm>
#include "block.h"
//TODO remove std namespace

using namespace std;
typedef size_t size_type;
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
    unsigned int * convert_to_parts(pointer row,size_type count){
        unsigned int *ret_arr = new unsigned int [3*count];
        for (size_type i = 0 ; i < count ; i++) {
            type_parts conv_float;
            conv_float.num = *row++;
            ret_arr[i] = conv_float.parts.sign;
            ret_arr[i+count] = conv_float.parts.exponent;
            ret_arr[i+count*2] = conv_float.parts.mantissa;
        }
        return ret_arr;
    }
}

