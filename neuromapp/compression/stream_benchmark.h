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

//local code for inclusion
#include "compression/allocator.h"
#include "compression/timer_tool.h"
#include "compression/exception.h"
#include "compression/block.h"
using neuromapp::block;
using neuromapp::cstandard;
using neuromapp::Timer;
using namespace std;
typedef size_t size_type;

namespace neuromapp {

    template <typename value_type,typename allocator_type>
        class stream_bench {
            typedef value_type * pointer;
            bool compress;
            Timer time_it;
            /*comment out the compress line to enable/disable the compression options*/
            /*this is the number of elements generated for filling the block*/
            const static size_type block_size =  8000;
            const static int vect_size = 640;
            /*this is the number of times that we run each benchmark computation before taking the minimum time*/
            const static int num_rounds =  10;
            block<value_type,allocator_type> v_a[vect_size];
            block<value_type,allocator_type> v_b[vect_size];
            block<value_type,allocator_type> v_c[vect_size];
            // calculation results section
            double mem_used, copy_bandwith , scale_bandwith, add_bandwith, triad_bandwith;
            public:
            //stream_bench (bool compress_opt) : compress {compress_opt},v_a{vect_size},v_b{vect_size},v_c{vect_size} {
            stream_bench (bool compress_opt) : compress {compress_opt}{
                if (compress) 
                    std::cout << "using compress" << std::endl;
                else
                    std::cout << "no compress" << std::endl;
#pragma omp parallel for
                for(int i = 0; i < vect_size;i++) {
                    block<value_type,allocator_type> ba(block_size);
                    ba.fill_block(1.0);
                    block<value_type,allocator_type> bb(block_size);
                    bb.fill_block(2.0);
                    block<value_type,allocator_type> bc(block_size);
                    bc.fill_block(0.0);
                    if (compress){
                        ba.compress();
                        bb.compress();
                        bc.compress();
                    }
                    v_a[i] = ba;
                    v_b[i] = bb;
                    v_c[i] = bc;
                }
            }

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
            void copy_benchmark () {
                std::cout << "begin copy benchmark" << std::endl;
                mem_used = vect_size*v_a[0].memory_allocated()*2*pow(10,-6);
                //prepare for the copy operation
                double min_time;
                for (int round = 0; round < num_rounds ; round++) {
                    time_it.start();
                    #pragma omp parallel for
                    for (int i = 0; i < vect_size;i++) {
                        if(compress) {
                            v_a[i].uncompress();
                            v_b[i].uncompress();
                        }
                        block<value_type,allocator_type> & a = v_a[i];
                        block<value_type,allocator_type> & b = v_b[i];
                        pointer  ptr_a = a.data();
                        pointer  ptr_b = b.data();
                        for (int j=0; j <(int) block_size;j++) {
                            ptr_a[j] = ptr_b[j];
                        }
                        if(compress) {
                            v_a[i].compress();
                            v_b[i].compress();
                        }
                    }
                    time_it.end();
                    if (round == 0) min_time = time_it.duration();
                    else if(min_time > time_it.duration()) min_time = time_it.duration();
                    copy_bandwith =  mem_used *(1000/min_time) ; // this will be in MBs
                }
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
            void scale_benchmark() {
                std::cout << "begin scale benchmark" << std::endl;
                double min_time;
                //scale operation
                for (int round = 0; round < num_rounds ; round++) {
                    time_it.start();
#pragma omp parallel for
                    for (int i = 0; i < vect_size;i++) {
                        if (compress) {
                            v_a[i].uncompress();
                            v_b[i].uncompress();
                        }
                        block<value_type,allocator_type> & a = v_a[i];
                        block<value_type,allocator_type> & b = v_b[i];
                        pointer  ptr_a = a.data();
                        pointer  ptr_b = b.data();
                        value_type scale = 5;
                        for (int j=0; j < (int) block_size;j++) {
                            ptr_a[j] = scale*ptr_b[j];
                        }
                        if (compress) {
                            v_a[i].compress();
                            v_b[i].compress();
                        }
                    }
                    time_it.end();
                    if (round == 0) min_time = time_it.duration();
                    else if(min_time > time_it.duration()) min_time = time_it.duration();
                }
                scale_bandwith = mem_used*(1000/min_time) ; // this will be in MBs
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
            void add_benchmark () {

                double min_time;
                std::cout << "begin add benchmark" << std::endl;
                //prepare for the add operation
                mem_used = vect_size*v_a[0].memory_allocated()*3*pow(10,-6);
                for (int round = 0; round < num_rounds ; round++) {
                    time_it.start();
#pragma omp parallel for
                    for (int i = 0; i < vect_size;i++) {
                        if (compress) {
                            v_a[i].uncompress();
                            v_b[i].uncompress();
                            v_c[i].uncompress();
                        }
                        block<value_type,allocator_type> & a = v_a[i];
                        block<value_type,allocator_type> & b = v_b[i];
                        block<value_type,allocator_type> & c = v_c[i];
                        pointer  ptr_a = a.data();
                        pointer  ptr_b = b.data();
                        pointer  ptr_c = c.data();
                        for (int j=0; j <(int) block_size;j++) {
                            ptr_a[j] = ptr_b[j] + ptr_c[j];
                        }
                        if (compress) {
                            v_a[i].compress();
                            v_b[i].compress();
                            v_c[i].compress();
                        }
                    }
                    time_it.end();
                    if (round == 0) min_time = time_it.duration();
                    else if(min_time > time_it.duration()) min_time = time_it.duration();
                }
                add_bandwith = mem_used*(1000/min_time) ; // this will be in MBs
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
            void triad_benchmark() {
                std::cout << "begin triad benchmark" << std::endl;
                double min_time;
                //triad operation
                for (int round = 0; round < num_rounds ; round++) {
                    time_it.start();
#pragma omp parallel for
                    for (int i = 0; i < vect_size;i++) {
                        if (compress) {
                            v_a[i].uncompress();
                            v_b[i].uncompress();
                            v_c[i].uncompress();
                        }
                        block<value_type,allocator_type> & a = v_a[i];
                        block<value_type,allocator_type> & b = v_b[i];
                        block<value_type,allocator_type> & c = v_c[i];
                        pointer  ptr_a = a.data();
                        pointer  ptr_b = b.data();
                        pointer  ptr_c = c.data();
                        value_type scale = 5;
                        for (int j=0; j < (int) block_size;j++) {
                            ptr_a[j] = scale*ptr_b[j] + ptr_c[j];
                        }
                        if (compress) {
                            v_a[i].compress();
                            v_b[i].compress();
                            v_c[i].compress();
                        }
                    }
                    time_it.end();
                    if (round == 0) min_time = time_it.duration();
                    else if(min_time > time_it.duration()) min_time = time_it.duration();
                }
                triad_bandwith = mem_used*(1000/min_time) ; // this will be in MBs
            }

            /**
            * run_stream_benchmark 
            *
            *
            * @brief
            *
            * @param 
            *
            * @return void
            */
            void run_stream_benchmark() {
                copy_benchmark () ;
                scale_benchmark() ;
                add_benchmark () ;
                triad_benchmark() ;
            }

            /**
            * output_results 
            *
            *
            * @brief
            *
            * @param 
            *
            * @return void
            */
            void output_results() {
                std::cout << left;
                std::cout << setw(20) <<  "operation: copy " <<setw(13) << "bandwith : " <<setw(16) <<   setprecision(5) << copy_bandwith << setw(5) << "MBs" << std::endl;
                std::cout<< setw(20) << "operation: add " << setw(13) << "bandwith : " <<setw(16) <<   setprecision(5) << add_bandwith << setw(5) << "MBs" << std::endl;
                std::cout<< setw(20) << "operation: scale " << setw(13) << "bandwith : " <<setw(16) << setprecision(5) << scale_bandwith << setw(5) << "MBs" << std::endl;
            }


    };
}

#endif
