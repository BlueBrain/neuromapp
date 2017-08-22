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
#include <iomanip>
#include <iterator>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include "compression/block.h"
#include "compression/conv_info.h"

using neuromapp::block;
typedef size_t size_type;

namespace neuromapp {


template <typename value_type>
struct insert_minder { 
    int v_i;    
    int pos;
    int rel_start;// relative starting position
    int shift_type;
    public:
    insert_minder(int rel_start_arg,int shift_type_arg) :rel_start {rel_start_arg},shift_type{shift_type_arg} {
        v_i = floor(rel_start/Conv_info<value_type>::total);// tells us which element in storage vector we start adding into
        pos = (Conv_info<value_type>::total-1)-rel_start%Conv_info<value_type>::total;// what position in that vector element to add 
    }
    void add_in(typename Conv_info<value_type>::bytetype * blk_ptr, typename Conv_info<value_type>::bytetype val ) {
        for (int i = 1; i <= shift_type;i++) {
            typename Conv_info<value_type>::bytetype frame_val = (val >> (shift_type -i)) & 1 ;
            *(blk_ptr + v_i) += frame_val << pos ;
            pos--;
            if (pos < 0) {// move to the start of the next number
                v_i++;
                pos = Conv_info<value_type>::total-1;
            }
        }
    }

    void reset() {
        v_i = floor(rel_start/Conv_info<value_type>::total);// tells us which element in storage vector we start adding into
        pos = (Conv_info<value_type>::total-1)-rel_start%Conv_info<value_type>::total;// what position in that vector element to add 
    }


    void take_out(typename Conv_info<value_type>::bytetype * blk_ptr,typename Conv_info<value_type>::bytetype & outcome_num) {

        for (int i = 1; i <= shift_type;i++) {
            outcome_num = outcome_num << 1;
            typename Conv_info<value_type>::bytetype frame_val = (*(blk_ptr +v_i) >> pos) & 1 ;
            outcome_num += frame_val ;
            pos--;
            if (pos < 0) {// move to the start of the next number
                v_i++;
                pos = Conv_info<value_type>::total-1;
            }
        }
    }

};

template <typename value_type>
struct cbit_holder {};

template <>
struct cbit_holder<float> {
    union {
        float val;
        uint32_t bits;
    }conv_bits;
};


template <>
struct cbit_holder<double> {
    union {
        double val;
        uint64_t bits;
    }conv_bits;
};



template <typename T>
typename Conv_info<T>::bytetype get_sign(T val) {
    cbit_holder<T> cb;
    cb.conv_bits.val = val;
    typename Conv_info<T>::bytetype mask = 1;
    typename Conv_info<T>::bytetype shift = Conv_info<T>::total - Conv_info<T>::sign_size;
    return (cb.conv_bits.bits >> shift) & mask  ; 
}

template <typename T>
typename Conv_info<T>::bytetype get_exp(T val) {
    cbit_holder<T> cb;
    cb.conv_bits.val = val;
    typename Conv_info<T>::bytetype mask = (1 << Conv_info<T>::exp_size)-1;
    typename Conv_info<T>::bytetype shift = Conv_info<T>::total - Conv_info<T>::exp_size - Conv_info<T>::sign_size;
    return (cb.conv_bits.bits >> shift) &  mask ; 
}


template <typename T>
typename Conv_info<T>::bytetype get_mant(T val) {
    cbit_holder<T> cb;
    cb.conv_bits.val=val;
    typename Conv_info<T>::bytetype mask = (1 << Conv_info<T>::mant_size)-1;
    return cb.conv_bits.bits & mask; 
}


template <typename value_type>
value_type get_dec(typename Conv_info<value_type>::bytetype bits){
    cbit_holder<value_type> cb;
    cb.conv_bits.bits=bits;
    return cb.conv_bits.val; 
}





    template <typename value_type,typename allocator_type>
            /**
            * convert_to_parts 
            *
            *
            * @brief Convert each element in the block pointed to by row_ptr into parts within the split block.
            *
            * @param value_type * row_ptr,block<typename Conv_info<value_type>::bytetype,allocator_type> & split_block,size_type row_num
            *
            * @return void
            */
        void convert_to_parts(value_type * unsplit_ptr,block<typename Conv_info<value_type>::bytetype,allocator_type> & split_block){
            int cells = split_block.dim0()* split_block.num_rows();

            insert_minder<value_type> sign_inserter(0,Conv_info<value_type>::sign_size);
            insert_minder<value_type> exp_inserter(cells,Conv_info<value_type>::exp_size);
            insert_minder<value_type> mant_inserter(Conv_info<value_type>::exp_size*cells+cells,Conv_info<value_type>::mant_size);
            typename Conv_info<value_type>::bytetype sign; 
            typename Conv_info<value_type>::bytetype exp; 
            typename Conv_info<value_type>::bytetype mant;
            typename Conv_info<value_type>::bytetype * split_blk_ptr = split_block.data();
            for (int i = 0 ; i < cells; i++) {
                value_type ele = *unsplit_ptr++;
                sign = get_sign(ele);
                sign_inserter.add_in(split_blk_ptr,sign);
                exp = get_exp(ele);
                exp_inserter.add_in(split_blk_ptr,exp);
                mant = get_mant(ele);
                mant_inserter.add_in(split_blk_ptr,mant);
            }
        }

    template <typename value_type,typename allocator_type>
            /**
            * convert_from_parts 
            *
            *
            * @brief Reciprocal function to teh one above, take values from the split block, and convert them back to their floating point representation
            * and stores them back in the unsplit block row value_type 
            *
            * @param value_type * row_ptr,block<typename Conv_info<value_type>::bytetype,allocator_type> & split_block,size_type row_num
            *
            * @return void
            */
    void convert_from_parts(value_type * unsplit_ptr,block<typename Conv_info<value_type>::bytetype,allocator_type> & split_block){
            int cells = split_block.dim0()* split_block.num_rows();
        insert_minder<value_type> sign_inserter(0,Conv_info<value_type>::sign_size);
        insert_minder<value_type> exp_inserter(cells,Conv_info<value_type>::exp_size);
        insert_minder<value_type> mant_inserter(Conv_info<value_type>::exp_size*cells+cells,Conv_info<value_type>::mant_size);
        typename Conv_info<value_type>::bytetype * split_blk_ptr = split_block.data();
        for (int i = 0;i< cells ; i++) {
            typename Conv_info<value_type>::bytetype aggregate = 0;
            sign_inserter.take_out(split_blk_ptr,aggregate);
            exp_inserter.take_out(split_blk_ptr,aggregate);
            mant_inserter.take_out(split_blk_ptr,aggregate);
            *unsplit_ptr++= get_dec<value_type>(aggregate); 
        }
    }

    template<typename value_type,typename allocator_type>
            /**
            * generate_split_block 
            *
            *
            * @brief The toplevel function that creates a split block of the right size, and calls the populating function.
            *
            * @param block<value_type,allocator_type> & unsplit_block
            *
            * @return block<typename Conv_info<value_type>::bytetype, allocator_type>
            */
        block<typename Conv_info<value_type>::bytetype, allocator_type> generate_split_block(block<value_type,allocator_type> & unsplit_block) {
            size_type rows_ = unsplit_block.num_rows();
            size_type cols_=unsplit_block.dim0();
            block<typename Conv_info<value_type>::bytetype,allocator_type> split_block(cols_,rows_);
            convert_to_parts(unsplit_block.data(),split_block);
            return split_block;
        }

    template<typename value_type,typename allocator_type>
            /**
            * generate_unsplit_block 
            *
            *
            * @brief The toplevel function that creates a unsplit block of the right size, and calls the populating function.
            *
            * @param block<typename Conv_info<value_type>::bytetype,allocator_type> & split_block
            *
            * @return block<value_type,allocator_type>
            */
        block<value_type,allocator_type> generate_unsplit_block(block<typename Conv_info<value_type>::bytetype,allocator_type> & split_block) {
            size_type rows_ = split_block.num_rows();
            size_type cols_=split_block.dim0();
            block<value_type,allocator_type> unsplit_block(cols_,rows_);
            convert_from_parts(unsplit_block.data(),split_block);
            return unsplit_block;
        }
}

#endif
