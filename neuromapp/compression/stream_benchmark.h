/*
 * Neuromapp - stream_benchmark.h, Copyright (c), 2015,
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
#ifndef STREAM_BENCH_H
#define STREAM_BENCH_H
#include <iostream>
#include <typeinfo>
#include <cmath>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <limits>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <boost/program_options.hpp>

//local code for inclusion
#include "compression/allocator.h"
#include "compression/block_sort.h"
#include "compression/bit_shifting.h"
#include "compression/conv_info.h"
#include "compression/timer_tool.h"
#include "compression/exception.h"
#include "compression/block.h"

namespace po = boost::program_options;
using neuromapp::block;
using neuromapp::cstandard;
using neuromapp::Timer;
using namespace std;
typedef size_t size_type;

const int num_rounds =10;
Timer time_it;

namespace neuromapp {

    //space for the alternative subroutine classes
    template <typename value_type,typename allocator_type> 
        class binary_stream_vectors {
            typedef typename Conv_info<value_type>::bytetype binary_rep;
            const static size_type block_size =  8000;
            size_type block_mem_size;
            bool compress_opt = false;
            const static int vect_size = 640;
            /*this is the number of times that we run each benchmark computation before taking the minimum time*/
            vector<block<binary_rep,allocator_type>> v_a;
            vector<block<binary_rep,allocator_type>> v_b;
            vector<block<binary_rep,allocator_type>> v_c;
            // calculation results section
            public:
            binary_stream_vectors(bool compress_arg) : compress_opt{compress_arg}  {
//#pragma omp parallel for
                for (int i = 0 ; i < vect_size; i++) {
                    block<value_type,allocator_type> ba(block_size);
                    ba.fill_block(1.0);
                    block<binary_rep,allocator_type> bin_ba = generate_split_block(ba);
                    if (i==0) block_mem_size = ba.memory_allocated();
                    block<value_type,allocator_type> bb(block_size);
                    bb.fill_block(2.0);
                    block<binary_rep,allocator_type> bin_bb = generate_split_block(bb);
                    block<value_type,allocator_type> bc(block_size);
                    bc.fill_block(0.0);
                    block<binary_rep,allocator_type> bin_bc = generate_split_block(bc);
                    if (compress_opt) {
                        bin_ba.compress();
                        bin_bb.compress();
                        bin_bc.compress();
                    }
                    v_a.push_back(bin_ba);
                    v_b.push_back(bin_bb);
                    v_c.push_back(bin_bc);
                }
            }


            inline int get_vec_size() {
                return vect_size;
            }
            inline int get_mem_size() {
                return vect_size*block_mem_size;
            }

            inline size_type get_block_size() {
                return block_size;
            }
            inline void va_i_comp(int i){
                if(compress_opt) v_a[i].compress();
            }
            inline void vb_i_comp(int i){
                if(compress_opt) v_b[i].compress();
            }
            inline void vc_i_comp(int i){
                if(compress_opt) v_c[i].compress();
            }

            block<value_type,allocator_type> va_i(int i) {
                if(compress_opt) {
                    v_a[i].uncompress();
                    return generate_unsplit_block<value_type,allocator_type>(v_a[i]);
                }
                return generate_unsplit_block<value_type,allocator_type>(v_a[i]);
            }
            block<value_type,allocator_type> vb_i(int i) {
                if(compress_opt) {
                    v_b[i].uncompress();
                    return generate_unsplit_block<value_type,allocator_type>(v_b[i]);
                }
                return generate_unsplit_block<value_type,allocator_type>(v_b[i]);
            }
            block<value_type,allocator_type> vc_i(int i) {
                if(compress_opt) {
                    v_c[i].uncompress();
                    return generate_unsplit_block<value_type,allocator_type>(v_c[i]);
                }
                return generate_unsplit_block<value_type,allocator_type>(v_c[i]);
            }
        };

    template <typename value_type,typename allocator_type> 
        class stream_vectors {
            const static size_type block_size =  8000;
            size_type block_mem_size;
            bool compress_opt;
            const static int vect_size = 640;
            /*this is the number of times that we run each benchmark computation before taking the minimum time*/
            vector<block<value_type,allocator_type>> v_a;
            vector<block<value_type,allocator_type>> v_b;
            vector<block<value_type,allocator_type>> v_c;
            // calculation results section
            public:
            stream_vectors (bool compress_arg) : compress_opt{compress_arg} {
//#pragma omp parallel for
                for (int i = 0 ; i < vect_size; i++) {
                    block<value_type,allocator_type> ba(block_size);
                    ba.fill_block(1.0);
                    if (i==0) block_mem_size = ba.memory_allocated();
                    block<value_type,allocator_type> bb(block_size);
                    bb.fill_block(2.0);
                    block<value_type,allocator_type> bc(block_size);
                    bc.fill_block(0.0);
                    if (compress_opt) {
                        ba.compress();
                        bb.compress();
                        bc.compress();
                    }
                    v_a.push_back(ba);
                    v_b.push_back(bb);
                    v_c.push_back(bc);
                }
            }
            inline size_type get_block_size() {
                return block_size;
            }
            inline int get_mem_size() {
                return vect_size*v_a[0].memory_allocated();
            }


            inline int get_vec_size() {
                return vect_size;
            }
            inline void va_i_comp(int i){
                if(compress_opt) v_a[i].compress();
            }
            inline void vb_i_comp(int i){
                if(compress_opt) v_b[i].compress();
            }
            inline void vc_i_comp(int i){
                if(compress_opt) v_c[i].compress();
            }


            //should these returns be references?
            inline block<value_type,allocator_type> va_i(int i){
                if(compress_opt)  {
                    v_a[i].uncompress();
                    return v_a[i];
                }
                return (v_a[i]);
            }
            inline block<value_type,allocator_type> vb_i(int i){
                if(compress_opt)  {
                    v_b[i].uncompress();
                    return v_b[i];
                }
                return (v_b[i]);
            }
            inline block<value_type,allocator_type> vc_i(int i){
                if(compress_opt)  {
                    v_c[i].uncompress();
                    return v_c[i];
                }
                return (v_c[i]);
            }
        };






    /**
     * copy_benchmark 
     *
     *
     * @brief
     *
     * @param 
     *
     * @return void
     */
    template <typename vect_types,typename value_type,typename allocator_type>
    void copy_benchmark (vect_types & vectors) {
        std::cout << "begin copy benchmark" << std::endl;
        double mem_used = vectors.get_mem_size()*2*pow(10,-6);
        //prepare for the copy operation
        double min_time;
        for (int round = 0; round < num_rounds ; round++) {
            time_it.start();
#pragma omp parallel for
            for (int i = 0; i <vectors.get_vec_size();i++) {
                block<value_type,allocator_type>  a = vectors.va_i(i);
                block<value_type,allocator_type>  b = vectors.vb_i(i); 
                value_type *  ptr_a = a.data();
                value_type *  ptr_b = b.data();
                for (int j=0; j <(int) vectors.get_block_size();j++) {
                    ptr_a[j] = ptr_b[j];
                }
                //depending on the vectors class constructor, this may not perform compression
                vectors.va_i_comp(i);   
                vectors.vb_i_comp(i);   
            }
            time_it.end();
            if (round == 0) min_time = time_it.duration();
            else if(min_time > time_it.duration()) min_time = time_it.duration();
        }
        double copy_bandwith =  mem_used *(1000/min_time) ; // this will be in MBs
        std::cout << left;
        std::cout << setw(20) <<  "operation: copy " <<setw(13) << "bandwith : " <<setw(16) <<   setprecision(5) << copy_bandwith << setw(5) << "MBs" << std::endl;
    }

    /**
     * scale_benchmark 
     *
     *
     * @brief
     *
     * @param 
     *
     * @return void
     */
    template <typename vect_types,typename value_type,typename allocator_type>
    void scale_benchmark(vect_types & vectors) {
        double mem_used = vectors.get_mem_size()*2*pow(10,-6);
        std::cout << "begin scale benchmark" << std::endl;
        double min_time;
        //scale operation
        for (int round = 0; round < num_rounds ; round++) {
            time_it.start();
#pragma omp parallel for
            for (int i = 0; i <vectors.get_vec_size();i++) {
                block<value_type,allocator_type>  a = vectors.va_i(i);
                block<value_type,allocator_type>  b = vectors.vb_i(i);
                value_type *  ptr_a = a.data();
                value_type *  ptr_b = b.data();
                value_type scale = 5;
                for (int j=0; j < (int) vectors.get_block_size();j++) {
                    ptr_a[j] = scale*ptr_b[j];
                }
                // recompression depending on the run
                vectors.va_i_comp(i);
                vectors.vb_i_comp(i);
            }
            time_it.end();
            if (round == 0) min_time = time_it.duration();
            else if(min_time > time_it.duration()) min_time = time_it.duration();
        }
        double scale_bandwith = mem_used*(1000/min_time) ; // this will be in MBs
        std::cout<< setw(20) << "operation: scale " << setw(13) << "bandwith : " <<setw(16) << setprecision(5) << scale_bandwith << setw(5) << "MBs" << std::endl;
    }

    /**
     * add_benchmark 
     *
     *
     * @brief
     *
     * @param 
     *
     * @return void
     */
    template <typename vect_types,typename value_type,typename allocator_type>
    void add_benchmark (vect_types & vectors) {

        double min_time;
        std::cout << "begin add benchmark" << std::endl;
        //prepare for the add operation
        double mem_used = vectors.get_mem_size()*3*pow(10,-6);
        for (int round = 0; round < num_rounds ; round++) {
            time_it.start();
#pragma omp parallel for
            for (int i = 0; i <vectors.get_vec_size();i++) {
                block<value_type,allocator_type>  a = vectors.va_i(i);
                block<value_type,allocator_type>  b = vectors.vb_i(i);
                block<value_type,allocator_type>  c = vectors.vc_i(i);
                value_type *  ptr_a = a.data();
                value_type *  ptr_b = b.data();
                value_type *  ptr_c = c.data();
                for (int j=0; j <(int) vectors.get_block_size();j++) {
                    ptr_a[j] = ptr_b[j] + ptr_c[j];
                }
                vectors.va_i_comp(i);   
                vectors.vb_i_comp(i);   
                vectors.vc_i_comp(i);   
            }
            time_it.end();
            if (round == 0) min_time = time_it.duration();
            else if(min_time > time_it.duration()) min_time = time_it.duration();
        }
        double add_bandwith = mem_used*(1000/min_time) ; // this will be in MBs
        std::cout<< setw(20) << "operation: add " << setw(13) << "bandwith : " <<setw(16) <<   setprecision(5) << add_bandwith << setw(5) << "MBs" << std::endl;
    }

    /**
     * triad_benchmark 
     *
     *
     * @brief
     *
     * @param 
     *
     * @return void
     */
    template <typename vect_types,typename value_type,typename allocator_type>
    void triad_benchmark(vect_types & vectors) {
        std::cout << "begin triad benchmark" << std::endl;
        double min_time;
        double mem_used = vectors.get_mem_size()*3*pow(10,-6);
        //triad operation
        for (int round = 0; round < num_rounds ; round++) {
            time_it.start();
#pragma omp parallel for
            for (int i = 0; i <vectors.get_vec_size();i++) {

                block<value_type,allocator_type>  a = vectors.va_i(i);
                block<value_type,allocator_type>  b = vectors.vb_i(i);
                block<value_type,allocator_type>  c = vectors.vc_i(i);

                value_type *  ptr_a = a.data();
                value_type *  ptr_b = b.data();
                value_type *  ptr_c = c.data();
                value_type scale = 5;
                for (int j=0; j < (int) vectors.get_block_size();j++) {
                    ptr_a[j] = scale*ptr_b[j] + ptr_c[j];
                }
                vectors.va_i_comp(i);   
                vectors.vb_i_comp(i);   
                vectors.vc_i_comp(i);   
            }
            time_it.end();
            if (round == 0) min_time = time_it.duration();
            else if(min_time > time_it.duration()) min_time = time_it.duration();
        }
        double triad_bandwith = mem_used*(1000/min_time) ; // this will be in MBs
        std::cout<< setw(20) << "operation: triad " << setw(13) << "bandwith : " <<setw(16) << setprecision(5) << triad_bandwith << setw(5) << "MBs" << std::endl;
    }
}

#endif
