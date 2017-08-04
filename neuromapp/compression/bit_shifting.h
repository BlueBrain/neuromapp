/*
 * Neuromapp - bit_shifting.h, Copyright (c), 2015,
 * Devin Bayly - University of Arizona
 * baylyd@email.arizona.edu,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#ifndef BIT_SHIFTING_H
#define BIT_SHIFTING_H
#include <iostream>
#include <string>
#include <algorithm>
#include "compression/block.h"


using namespace std;
using neuromapp::block;
typedef size_t size_type;

namespace neuromapp {
    /*base trait structs */
    template <class T>
        struct trait_mantissa{};
    template <class T>
        struct trait_exponent{};

    /*specializtions for the type converted from */
    template <>
        struct trait_mantissa<float> {
            const static int value = 23;
        };
    template <>
        struct trait_exponent<float> {
            const static int value = 8;
        };
    template <>
        struct trait_mantissa<double> {
            const static int value = 52;
        };
    template <>
        struct trait_exponent<double> {
            const static int value = 11;
        };

    template<class T>
        union conversion_parts {
            T num;
            struct {
                unsigned int mantissa = trait_mantissa<T>::value;
                unsigned int exponent = trait_exponent<T>::value;
                unsigned int sign:1;
            } parts;
        } ;


    template <typename value_type,typename allocator_type>

        /*
         * and creating an unsigned int array that represents its parts
         */
            /**
            * convert_to_parts 
            *
            *
            * @brief
            *
            * @param value_type * row_ptr,block<unsigned int,allocator_type> & split_block,size_type row_num
            *
            * @return void
            */
        void convert_to_parts(value_type * row_ptr,block<unsigned int,allocator_type> & split_block,size_type row_num){
            size_type count = split_block.dim0()/3;
            for (size_type i = 0 ; i < count ; i++) {
                conversion_parts<value_type> conv_float{*row_ptr++};
                split_block(i,row_num) = conv_float.parts.sign;
                split_block(i+count,row_num) = conv_float.parts.exponent;
                split_block(i+count*2,row_num) = conv_float.parts.mantissa;
            }
        }

    template <typename value_type,typename allocator_type>
        /*reciprocal function to teh one above, take values from the split block, and convert them back to their floating point representation
         * and store them back in the unsplit block row value_type *
         */
            /**
            * convert_from_parts 
            *
            *
            * @brief
            *
            * @param value_type * row_ptr,block<unsigned int,allocator_type> & split_block,size_type row_num
            *
            * @return void
            */
        void convert_from_parts(value_type * row_ptr,block<unsigned int,allocator_type> & split_block,size_type row_num){
            size_type count = split_block.dim0()/3;
            for (size_type i = 0 ; i < count ; i++) {
                conversion_parts<value_type> conv_float;
                conv_float.parts.sign=split_block(i,row_num) ;
                conv_float.parts.exponent=split_block(i+count,row_num) ;
                conv_float.parts.mantissa=split_block(i+count*2,row_num) ;
                *row_ptr++ = conv_float.num;
            }
        }

    template<typename value_type,typename allocator_type>
            /**
            * populate_split_block 
            *
            *
            * @brief
            *
            * @param  block<unsigned int,allocator_type> & split_block,block<value_type,allocator_type>& unsplit_block
            *
            * @return void
            */
        void populate_split_block( block<unsigned int,allocator_type> & split_block,block<value_type,allocator_type>& unsplit_block) {
            size_type row_limit = unsplit_block.num_rows();
            for (size_type row_num = 0; row_num < row_limit;row_num++) {
                convert_to_parts(&unsplit_block(0,row_num),split_block,row_num);
            }
        }

    template<typename value_type,typename allocator_type>
            /**
            * populate_unsplit_block 
            *
            *
            * @brief
            *
            * @param  block<unsigned int,allocator_type> & split_block,block<value_type,allocator_type>& unsplit_block
            *
            * @return void
            */
        void populate_unsplit_block( block<unsigned int,allocator_type> & split_block,block<value_type,allocator_type>& unsplit_block) {
            size_type row_limit = unsplit_block.num_rows();
            for (size_type row_num = 0; row_num < row_limit;row_num++) {
                convert_from_parts(&unsplit_block(0,row_num),split_block,row_num);
            }
        }


    template<typename value_type,typename allocator_type>
            /**
            * generate_split_block 
            *
            *
            * @brief
            *
            * @param block<value_type,allocator_type> & unsplit_block
            *
            * @return block<unsigned int, allocator_type>
            */
        block<unsigned int, allocator_type> generate_split_block(block<value_type,allocator_type> & unsplit_block) {
            size_type rows_ = unsplit_block.num_rows(),cols_=unsplit_block.dim0();
            block<unsigned int,allocator_type> split_block(cols_,rows_);
            populate_split_block(split_block,unsplit_block);
            return split_block;
        }

    template<typename value_type,typename allocator_type>
            /**
            * generate_unsplit_block 
            *
            *
            * @brief
            *
            * @param block<unsigned int,allocator_type> & split_block
            *
            * @return block<value_type,allocator_type>
            */
        block<value_type,allocator_type> generate_unsplit_block(block<unsigned int,allocator_type> & split_block) {
            size_type rows_ = split_block.num_rows(),cols_=split_block.dim0();
            block<value_type,allocator_type> unsplit_block(cols_/3,rows_);
            populate_unsplit_block(split_block,unsplit_block);
            return unsplit_block;
        }
}

#endif
