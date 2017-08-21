/*
 * Neuromapp - kernel_measurements.h, Copyright (c), 2015,
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
#ifndef KERNEL_MEASUREMENT_H
#define KERNEL_MEASUREMENT_H
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <boost/program_options.hpp>

#include "compression/compressor.h"
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block_sort.h"
#include "compression/block.h"
#include "compression/bit_shifting.h"
#include "compression/timer_tool.h"
#include "compression/conv_info.h"

using namespace std; 
using neuromapp::col_sort;
using neuromapp::generate_split_block;
using neuromapp::generate_unsplit_block;
using neuromapp::block;
using neuromapp::Timer;
namespace po= boost::program_options;
typedef size_t size_type;


/* Section basic global variable instantiation  */

#define ARRAY_SIZE 640 


// section for the global vars
string fname;
namespace neuromapp {
    /* Section for the computation functions */
    template <typename allocator_type>
        struct level1_compute {
            public:
            /**
            * operator ()  
            *
            *
            * @brief This function simply performs an addition using the value the pointer (ptr) refers to, and the argument coef.
            *
            * @param ostream & os
            *
            * @return void 
            */
            void operator () (block<double,allocator_type> & blk,ostream & os) {
                
                os << " running lvl1 compute ";
                double * ptr = blk.data();
                for (int i = 0;i< blk.dim0()*blk.num_rows();i++) {
                    double result = *ptr +5.0;//arbitrary 5.0
                    ptr++;
                }
            }
        };




    template <typename allocator_type>
        struct level2_compute {
            double u_init = 1.0,x_init = 2.0;
            block<double ,allocator_type> u_block,x_block;
            public:
            /**
            * operator ()  
            *
            *
            * @brief The level2 computation is a scaled down version of the Tsodyks-Markram model of PSP adjustment due to activity. Two 1d blocks
            * are kept within the functor struct and have their contents updated by the computation.
            *
            * @param ostream & os
            *
            * @return void 
            */
            void operator () (block<double,allocator_type> & blk,ostream & os) {
                os << " running lvl2 compute ";
                //each ptr here is a row in the 2d block
                //except ptr1 which isn't necessary for use

            }
        };



    template <typename allocator_type>
        struct level3_compute {
            double y_initial=1.0,  t_initial=0.0, step = .0001, t_limit = 1000.0;
            public:
            /* this is the function that you should edit if you want a different solution */
            double differential (double y,double t) {
                return pow(y,2) + t*30;// totally arbitrary for the moment
            }
            /**
            * operator ()  
            *
            *
            * @brief The level 3function. This uses the Euler method to solve a particular differential equation, with given initial values, time limit, and a step size.
            *
            * @param ostream & os
            *
            * @return void 
            */
            void operator () (block<double,allocator_type> & blk,ostream & os){
                os << " running lvl3 compute ";
                /* treat the initials as initials for each step, so initial to start, and each step */
                while (t_initial < t_limit) {
                    y_initial = y_initial + differential(y_initial,t_initial) * (step);
                    t_initial +=step;
                }
            }
        };

    /* Section for sampling strategies */

    template< typename allocator_type >
            /**
            * init_array 
            *
            *
            * @brief This function builds our array of blocks using the specific size ARRAY_SIZE. Each block receives the same file contents.
            *
            * @param block<double,allocator_type> * block_array
            *
            * @return void
            */
        void init_array (block<double,allocator_type> * block_array,po::variables_map vm) {

            ifstream blk_file;
            for (int i = 0; i < (int) ARRAY_SIZE; i++) {
                blk_file.open(fname);
                block<double,allocator_type> b1;
                blk_file >> b1;
                block_array[i] = b1;
                block_array[i].compress();
                blk_file.close();
                blk_file.clear();
            }
        }

    template< typename allocator_type >
            /**
            * init_array 
            *
            *
            * @brief This function builds our array of blocks using the specific size ARRAY_SIZE. Each block receives the same file contents.
            *
            * @param block<double,allocator_type> * block_array
            *
            * @return void
            */
        void bin_init_array (block<typename Conv_info<double>::bytetype,allocator_type> * block_array,po::variables_map vm) {
            ifstream blk_file;
            for (int i = 0; i < (int) ARRAY_SIZE; i++) {
                blk_file.open(fname);
                block<double,allocator_type> b1;
                blk_file >> b1;
                block_array[i] = generate_split_block(b1);
                block_array[i].compress();
                blk_file.close();
                blk_file.clear();
            }
        }



    /* selects a novel position in a representative array  that hasn't been picked already */
    template <size_type SIZE>
        class Block_selector {
            int selected_count = 0;
            int places [SIZE];
            public:
            Block_selector () {
                memset(places,0,SIZE*sizeof(int)); //zero out the places
            }
            /**
            * operator() 
            *
            *
            * @brief This function randomly selects 1 block from the array to be passed to the ensuing computations. Will stop after 10% of the 
            * blocks have been selected.
            *
            * @param 
            *
            * @return int 
            */
            int operator()() {
                if(selected_count > (int) SIZE/10)
                    return -1;
                else {
                    int place;
                    srand(time(0));// seed the random
                    while(places[place = rand() % SIZE]);
                    places[place]++;
                    selected_count++;
                    return place;
                }
            }
        };



    /* section for the these things combined */
    template <typename allocator_type,typename fun_ob>
            /**
            * kernel_measure 
            *
            *
            * @brief This is the main event. Time counts are taken for computations that involve all three levels using blocks that feature a compression step, and those that don't. Ultimately the result is given back to the user in the form of a table printed out to the screen.
            *
            * @param fun_ob & f,ostream & os,block<double,allocator_type> & blk
            *
            * @return void
            */
        void kernel_measure(fun_ob & f,ostream & os,block<double,allocator_type> & blk) {
            /*create and start the timer,compress run first */
            Timer time_it; time_it.start();
            os << " compression: " ;
            blk.uncompress();
            f(blk,os);
            blk.compress();
            time_it.end();
            os << " duration " << time_it.duration() ;
            blk.uncompress();
            /* now the non-compress run */
            os << " non-compress: " ;
            time_it.start();
            f(blk,os);
            time_it.end();
            os << " duration " << time_it.duration() << std::endl;
        }

    /* section for the these things combined */
    template <typename allocator_type,typename fun_ob>
            /**
            * kernel_measure 
            *
            *
            * @brief This is the main event. Time counts are taken for computations that involve all three levels using blocks that feature a compression step, and those that don't. Ultimately the result is given back to the user in the form of a table printed out to the screen.
            *
            * @param fun_ob & f,ostream & os,block<double,allocator_type> & blk
            *
            * @return void
            */
        void bin_kernel_measure(fun_ob & f,ostream & os,block<typename Conv_info<double>::bytetype,allocator_type> & blk) {
            /*create and start the timer,compress run first */
            Timer time_it; time_it.start();
            os << " compression: " ;
            blk.uncompress();
            block<double,allocator_type> dec_block= generate_unsplit_block<double,allocator_type>(blk);
            f(dec_block,os);
            blk.compress();
            time_it.end();
            os << " duration " << time_it.duration() ;
            /* now the non-compress run */
            os << " non-compress: " ;
            time_it.start();
            f(dec_block,os);
            time_it.end();
            os << " duration " << time_it.duration() << std::endl;
        }



    /* this function allows us to capture compress and non-compress runs using kernel_measure and the different levels of compute complexity */
    template <typename allocator_type>
            /**
            * option_coordinator 
            *
            *
            * @brief
            *
            * @param ostream & out, double coef,double step,double y_initial, double t_initial,double t_limit
            *
            * @return void
            */
        void option_coordinator(ostream & out, double coef,double step,double y_initial, double t_initial,double t_limit,po::variables_map vm) {
            Block_selector<(size_type) ARRAY_SIZE> bs;
            block<double,allocator_type> block_array[ARRAY_SIZE];
            init_array(block_array,vm);
            /* loop variables */
            int pos;
            /* continue to select blocks in positions, until 10% of the array has been used. atwhich point the -1 is returned */
            while((pos = bs()) != -1) {
                out << " position is : " << pos<< std::endl; 
                /* generate the initial computation states for functions */
                level1_compute<allocator_type> f_lvl1;
                level2_compute<allocator_type> f_lvl2;
                level3_compute<allocator_type> f_lvl3;
                kernel_measure(f_lvl1,out,block_array[pos]);
                kernel_measure(f_lvl2,out,block_array[pos]);
                kernel_measure(f_lvl3,out,block_array[pos]);
            }
        }

    template<typename allocator_type>
        void bin_option_coordinator(ostream & out, double coef,double step,double y_initial, double t_initial,double t_limit,po::variables_map vm) {
            Block_selector<(size_type) ARRAY_SIZE> bs;
            block<typename Conv_info<double>::bytetype,allocator_type> block_array[ARRAY_SIZE];
            bin_init_array(block_array,vm);
            /* loop variables */
            int pos;
            /* continue to select blocks in positions, until 10% of the array has been used. atwhich point the -1 is returned */
            while((pos = bs()) != -1) {
                out << " position is : " << pos<< std::endl; 
                /* generate the initial computation states for functions */
                level1_compute<allocator_type> f_lvl1;
                level2_compute<allocator_type> f_lvl2;
                level3_compute<allocator_type> f_lvl3;
                bin_kernel_measure(f_lvl1,out,block_array[pos]);
                bin_kernel_measure(f_lvl2,out,block_array[pos]);
                bin_kernel_measure(f_lvl3,out,block_array[pos]);
            }
        }

    template<typename allocator_type>
            /**
            * run_km 
            *
            *
            * @brief
            *
            * @param string & fname_arg
            *
            * @return void
            */
        //DECIDE ABOUT GLOBALS FOR SETTING BLOCK PRECOMPRESSION OPTIONS
        void bin_run_km (string & fname_arg,po::variables_map vm) {
            std::cout << " Starting Kernel Measure " << std::endl;
            fname=fname_arg;
            double coef = 2.0,
                   step = 1.0,
                   y_param = 2.0,
                   t_start = 0.0,
                   t_limit = 10000.0;
            bin_option_coordinator<allocator_type>(std::cout,coef,step,y_param,t_start,t_limit,vm);
        }
    template<typename allocator_type>
            /**
            * run_km 
            *
            *
            * @brief
            *
            * @param string & fname_arg
            *
            * @return void
            */
        //DECIDE ABOUT GLOBALS FOR SETTING BLOCK PRECOMPRESSION OPTIONS
        void run_km (string & fname_arg,po::variables_map vm) {
            std::cout << " Starting Kernel Measure " << std::endl;
            fname=fname_arg;
            double coef = 2.0,
                   step = 1.0,
                   y_param = 2.0,
                   t_start = 0.0,
                   t_limit = 10000.0;
            option_coordinator<allocator_type>(std::cout,coef,step,y_param,t_start,t_limit,vm);
        }

}

#endif
