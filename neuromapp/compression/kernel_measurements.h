/* Filename : kernel_measurements.h
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, Blue Brain Project
 * Purpose : xxx
 * Date : 2017-08-01 
 */
/* Filename : kernel_measurements.cpp
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, EPFL
 * Purpose : This file exists to assist in determining what level of computation is necessary to make compression routines 
 *          not significantly contribute to the total computation time. We will experiment with varying levels of complexity for 
 *          the calculations starting with simple STREAM like calculations, and gradually working up to Euler method differential 
 *          equation solving techniques.
 * Date : 2017-07-20 
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
#include "compressor.h"
#include "allocator.h"
#include "exception.h"
#include "block_sort.h"
#include "block.h"
#include "bit_shifting.h"
#include "timer_tool.h"

using neuromapp::col_sort;
using neuromapp::generate_split_block;
using neuromapp::block;
using neuromapp::Timer;
typedef size_t size_type;


/* Section basic global variable instantiation  */

#define ARRAY_SIZE 100 


// section for the global vars
string fname;
namespace neuromapp {
    /* Section for the computation functions */
    template <typename allocator_type>
        struct level1_compute {
            double coef,* ptr,* end;
            public:
            level1_compute(double * ptr_arg,double coef_arg,double * end_arg) 
                :coef {coef},ptr{ptr_arg}, end{end_arg} {}
            void operator () (ostream & os) {
                os << " running lvl1 compute ";
                while ( ptr != end) {
                    double result = *ptr +coef;
                    ptr++;
                }
            }
        };




    template <typename allocator_type>
        struct level2_compute {
            double coef, step, * end;
            vector<double* > row_ptrs;
            size_type cols;
            double u_init = 1.0,x_init = 2.0;
            block<double ,allocator_type> u_block,x_block;
            public:
            level2_compute(vector<double *> row_ptrs_arg,double coef_arg,double step_arg,size_type cols_arg,double * end_arg) 
                :  row_ptrs{row_ptrs_arg}, coef{coef_arg}, step{step_arg}, cols{cols_arg}, end {end_arg} {
                    //the +1's allow us to keep a "previous" value to refer to which is the dynamic blocks initial values
                    u_block.resize(cols+1);
                    x_block.resize(cols+1);
                    //initializing values
                    u_block(0) = u_init;
                    x_block(0) = x_init;
                }
            void operator () (ostream & os) {
                os << " running lvl2 compute ";
                //each ptr here is a row in the 2d block
                double * ptr1 = row_ptrs[0],* ptr2 = row_ptrs[2],* ptr3 = row_ptrs[2],* ptr4 = row_ptrs[3];
                double * u_ptr = u_block.begin()+1,* x_ptr = x_block.begin()+1;

                for(;ptr1 != end;ptr1++,ptr2++,ptr3++,ptr4++,u_ptr++,x_ptr++) {
                    /* now run the computation function on each element */
                    double U = *ptr1,tau_fac = *ptr3,tau_rec = *ptr2,u_last = *(u_ptr-1) ,x_last = *(x_ptr-1);
                    *u_ptr= U + u_last*(1-U)*exp(-step/tau_fac);
                    *x_ptr = 1+(x_last - x_last*u_last -1)*exp(-step/tau_rec);
                }
            }
        };



    template <typename allocator_type>
        struct level3_compute {
            double y_initial,  t_initial, step, t_limit;
            public:
            level3_compute (double y_initial_arg, double t_initial_arg,double step_arg,double t_limit_arg) :
                y_initial {y_initial_arg},  t_initial {t_initial_arg}, step {step_arg}, t_limit {t_limit_arg} {}
            /* this is the function that you should edit if you want a different solution */
        /**
        * differential 
        *
        *
        * @brief
        *
        * @param double y,double t
        *
        * @return double
        */
            double differential (double y,double t) {
                return pow(y,2) + t*30;// totally arbitrary for the moment
            }
            void operator () (ostream & os){
                os << " running lvl3 compute ";
                /* treat the initials as initials for each step, so initial to start, and each step */
                double y_next;
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
        * @brief
        *
        * @param block<double,allocator_type> * block_array
        *
        * @return void
        */
        void init_array (block<double,allocator_type> * block_array) {
            ifstream blk_file;
            for (int i = 0; i < (int) ARRAY_SIZE; i++) {
                blk_file.open(fname);
                block<double,allocator_type> b1;
                blk_file >> b1;
                block_array[i] = b1;
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
            int operator()() {
                if(selected_count > SIZE/10)
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
        * @brief
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
            f(os);
            blk.compress();
            time_it.end();
            os << " duration " << time_it.duration() ;
            blk.uncompress();
            /* now the non-compress run */
            os << " non-compress: " ;
            time_it.start();
            f(os);
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
        void option_coordinator(ostream & out, double coef,double step,double y_initial, double t_initial,double t_limit) {
            Block_selector<(size_type) ARRAY_SIZE> bs;
            block<double,allocator_type> block_array[ARRAY_SIZE];
            init_array(block_array);
            /* loop variables */
            int pos,count = 0;
            /* continue to select blocks in positions, until 10% of the array has been used. atwhich point the -1 is returned */
            while((pos = bs()) != -1) {
                out << " position is : " << pos<< std::endl; 
                size_type cols = block_array[pos].dim0();
                double * block_ptr = block_array[pos].begin(), * end = block_ptr + cols;
                vector<double * > ptr_vec{block_ptr,block_ptr+cols,block_ptr+2*cols,block_ptr+3*cols};
                /* generate the initial computation states for functions */
                level1_compute<allocator_type> f_lvl1(block_ptr,coef,end);
                level2_compute<allocator_type> f_lvl2(ptr_vec,coef,step,cols,end);
                level3_compute<allocator_type> f_lvl3(y_initial,t_initial,step,t_limit);
                kernel_measure(f_lvl1,out,block_array[pos]);
                kernel_measure(f_lvl2,out,block_array[pos]);
                kernel_measure(f_lvl3,out,block_array[pos]);
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
        void run_km (string & fname_arg) {
            ofstream out("kernel_measure.log");
            out << " Starting Kernel Measure " << std::endl;
            fname=fname_arg;
            double coef = 2.0,
                   step = 1.0,
                   y_param = 2.0,
                   t_start = 0.0,
                   t_limit = 10000.0;
            option_coordinator<allocator_type>(out,coef,step,y_param,t_start,t_limit);
        }

}
#endif
