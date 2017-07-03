#include <iostream>
#include <string>
#include <algorithm>
#include "block.h"
//TODO remove std namespace
//

using namespace std;
typedef size_t size_type;

namespace neuromapp {
    template <typename T>
    typedef union {
        T num;
        struct {
            unsigned int sign:1;
            unsigned int exponent:8;
            unsigned int mantissa:23;
        } parts;
    } type_parts;


    /*function for taking in an array of floats ( TODO make generic) 
     * and creating an unsigned int array that represents its parts
     */
    template <typename T>
    unsigned int[] convert_to_parts(T * row,size_type count){
        unsigned int ret_arr[3*count];
        for (size_type i = 0 ; i < count ; i++) {
            type_parts parts = {.num = *row};
            ret_arr[i] = parts.sign;
            ret_arr[i+count] = parts.exponent;
            ret_arr[i+count*2] = parts.mantissa;
        }
        return ret_arr;
    }
}

