/* Filename : stream_bench.h
 * Authors : Devin Bayly
 * Organization : University of Arizona
 * Purpose : xxx
 * Date : 2017-07-27 
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
#include "allocator.h"
#include "timer_tool.h"
#include "exception.h"
#include "block.h"
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
            const static int vect_size = 64;
            /*this is the number of times that we run each benchmark computation before taking the minimum time*/
            const static int num_rounds =  10;
            block<value_type,allocator_type> v_a[vect_size];
            block<value_type,allocator_type> v_b[vect_size];
            block<value_type,allocator_type> v_c[vect_size];
            // calculation results section
            double mem_used, copy_bandwith , scale_bandwith, add_bandwith,triad_bandwith;
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
                    block<value_type,allocator_type> bb(block_size);
                    block<value_type,allocator_type> bc(block_size);
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
                        for (int j=0; j < block_size;j++) {
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
                        for (int j=0; j < block_size;j++) {
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

            void add_benchmark () {

                double min_time;
                std::cout << "begin add benchmark" << std::endl;
                //prepare for the add operation
                mem_used = v_a[0].memory_allocated()*3*pow(10,-6);
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
                        for (int j=0; j < block_size;j++) {
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
                        for (int j=0; j < block_size;j++) {
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
                double triad_bandwith = mem_used*(1000/min_time) ; // this will be in MBs
            }

            void run_stream_benchmark() {
                triad_benchmark() ;
                add_benchmark () ;
                scale_benchmark() ;
                copy_benchmark () ;
            }

            void output_results() {
                std::cout << left;
                std::cout << setw(20) <<  "operation: copy " <<setw(13) << "bandwith : " <<setw(16) <<   setprecision(5) << copy_bandwith << setw(5) << "MBs" << std::endl;
                std::cout<< setw(20) << "operation: add " << setw(13) << "bandwith : " <<setw(16) <<   setprecision(5) << add_bandwith << setw(5) << "MBs" << std::endl;
                std::cout<< setw(20) << "operation: scale " << setw(13) << "bandwith : " <<setw(16) << setprecision(5) << scale_bandwith << setw(5) << "MBs" << std::endl;
                std::cout<< setw(20) << "operation: triad " << setw(13) << "bandwith : " <<setw(16) << setprecision(5) << triad_bandwith << setw(5) << "MBs" << std::endl;
            }


    };
}

#endif
