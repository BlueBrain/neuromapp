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
#include "compression/conv_info.h"
#include "compression/block_sort.h"
#include "compression/block.h"
#include "compression/bit_shifting.h"
#include "compression/timer_tool.h"

using neuromapp::col_sort;
using neuromapp::generate_split_block;
using neuromapp::block;
using neuromapp::Timer;
using namespace std;
namespace po= boost::program_options;
typedef size_t size_type;
typedef Conv_info<double>::bytetype binary_rep;


/* Section basic global variable instantiation  */

#define VECTOR_SIZE 100 


// section for the global vars
string fname;
namespace neuromapp {
    /* Section for the computation functions */
    template <typename value_type,typename allocator_type>
        struct level1_compute {
            double coef;
            public:
            level1_compute(double coef_arg) 
                :coef {coef_arg} {}
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
            void operator () (block<value_type,allocator_type> * b1,po::variables_map & vm,ostream & os) {
                os << " running lvl1 compute ";

                double * ptr;
                if (vm.count("split")) {
                    block<double,allocator_type> b2 = generate_unsplit_block(*b1);
                    ptr = b2.data();
                } else {
                    ptr = b1.data();
                }
                size_type end = b1->dim0();
                for (int i = 0 ; i < (int) end; i++)  {
                    double result = *ptr +coef;
                    ptr++;
                }
            }
        };





    template <typename value_type,typename allocator_type>
        struct level3_compute {
            double y_initial,  t_initial, step, t_limit;
            public:
            level3_compute (double y_initial_arg, double t_initial_arg,double step_arg,double t_limit_arg) :
                y_initial {y_initial_arg},  t_initial {t_initial_arg}, step {step_arg}, t_limit {t_limit_arg} {}
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
            void operator () (block<value_type,allocator_type> * b1,po::variables_map &vm,ostream & os){
                os << " running lvl3 compute ";
                /* treat the initials as initials for each step, so initial to start, and each step */
                while (t_initial < t_limit) {
                    y_initial = y_initial + differential(y_initial,t_initial) * (step);
                    t_initial +=step;
                }
            }
        };

    /* Section for sampling strategies */

    template<typename value_type,typename allocator_type >
            /**
            * init_vector 
            *
            *
            * @brief This function builds our vector of blocks using the specific size VECTOR_SIZE. Each block receives the same file contents.
            *
            * @param block<double,allocator_type> * block_vec
            *
            * @return void
            */
        void init_vector (vector<block<value_type,allocator_type>> & block_vec,po::variables_map & vm) {
            ifstream blk_file;
            for (int i = 0; i < (int) VECTOR_SIZE; i++) {
                blk_file.open(fname);
                block<double,allocator_type> b1;
                blk_file >> b1;
                //different storage if split option provided
                if (vm.count("split")) {
                    block<binary_rep,allocator_type> b2 = generate_split_block(b1);
                    block_vec.push_back(b2);
                }
                else block_vec.push_back(b1);
                block_vec[i].compress();
                blk_file.close();
                blk_file.clear();
            }
        }


    /* selects a novel position in a representative vector  that hasn't been picked already */
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
            * @brief This function randomly selects 1 block from the vector to be passed to the ensuing computations. Will stop after 10% of the 
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
    template <typename value_type,typename allocator_type,typename fun_ob>
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
        void kernel_measure(fun_ob & f,ostream & os,block<value_type,allocator_type> & blk,po::variables_map & vm) {
            /*create and start the timer,compress run first */
            Timer time_it; time_it.start();
            os << " compression: " ;
            blk.uncompress();
            f(&blk,vm,os);
            blk.compress();
            time_it.end();
            os << " duration " << time_it.duration() ;
            blk.uncompress();
            /* now the non-compress run */
            os << " non-compress: " ;
            time_it.start();
            f(&blk,vm,os);
            time_it.end();
            os << " duration " << time_it.duration() << std::endl;
        }

    /* this function allows us to capture compress and non-compress runs using kernel_measure and the different levels of compute complexity */
    template <typename value_type,typename allocator_type>
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
        void option_coordinator(vector<block<value_type,allocator_type>> & block_vec,ostream & out, double coef,double step,double y_initial, double t_initial,double t_limit,po::variables_map & vm) {
            if (vm.count("split")) init_vector<binary_rep,allocator_type>(block_vec,vm);
            else  init_vector<double,allocator_type>(block_vec,vm);
            Block_selector<(size_type) VECTOR_SIZE> bs;
            /* loop variables */
            int pos;
            /* continue to select blocks in positions, until 10% of the vector has been used. atwhich point the -1 is returned */
            while((pos = bs()) != -1) {
                out << " position is : " << pos<< std::endl; 
                block<value_type,allocator_type> blk = block_vec[pos];
                /* generate the initial computation states for functions */
                level1_compute<value_type,allocator_type> f_lvl1(coef);
                level3_compute<value_type,allocator_type> f_lvl3(y_initial,t_initial,step,t_limit);
                kernel_measure(f_lvl1,out,blk,vm);
                kernel_measure(f_lvl3,out,blk,vm);
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
        void run_km (string & fname_arg,po::variables_map & vm) {
            std::cout << " Starting Kernel Measure " << std::endl;
            double coef = 2.0,
                   step = 1.0,
                   y_param = 2.0,
                   t_start = 0.0,
                   t_limit = 10000.0;
            if (vm.count("split")) {
                vector<block<binary_rep,allocator_type>> block_vec(VECTOR_SIZE);
                option_coordinator<binary_rep,allocator_type>(block_vec,std::cout,coef,step,y_param,t_start,t_limit,vm);
            } else {
                vector<block<double,allocator_type>> block_vec(VECTOR_SIZE);
                option_coordinator<double,allocator_type>(block_vec,std::cout,coef,step,y_param,t_start,t_limit,vm);
            }

        }
}

#endif
