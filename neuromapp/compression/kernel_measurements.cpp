/* Filename : kernel_measurements.cpp
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, EPFL
 * Purpose : This file exists to assist in determining what level of computation is necessary to make compression routines 
 *          not significantly contribute to the total computation time. We will experiment with varying levels of complexity for 
 *          the calculations starting with simple STREAM like calculations, and gradually working up to Euler method differential 
 *          equation solving techniques.
 * Date : 2017-07-20 
 */

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

//TODO add filepath
#define FNAME "trans_data/values_8_a8780trans_bulk.csv"
#define ARRAY_SIZE 100 
#define COMPRESS

/* Section for the computation functions */

template <typename allocator_type>
struct level1_compute {
    void operator () (double * ptr,double coef,double step,size_type cols,double * end) {
        while (ptr != end) {
            /* now run the computation function on each element */
            double U = *ptr,tau_fac = *ptr + 3*cols;
            double value= U + coef*(1-U)*exp(-step/tau_fac);
            ptr++;
        }
    }
};

template <typename allocator_type>
struct level3_compute {
    double differential (double y,double t) {
        return pow(y,2) + t*30;// totally arbitrary for the moment
    }
    void operator () (double y_initial, double t_initial,double step,double t_limit) {
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
void init_array (block<double,allocator_type> * block_array) {
      ifstream blk_file;
      for (int i = 0; i < (int) ARRAY_SIZE; i++) {
          blk_file.open(FNAME);
          block<double,allocator_type> b1;
          blk_file >> b1;
#ifdef COMPRESS
          b1.compress();
#endif
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
void kernel_measure(fun_ob & f) {
    Timer time_it;
    Block_selector<(size_type) ARRAY_SIZE> bs;
    block<double,allocator_type> block_array[ARRAY_SIZE];
    init_array(block_array);
    double coef = 2.0;
    double step = 1.0;
    int pos;
    /* continue to select blocks in positions, until 10% of the array has been used. atwhich point the -1 is returned */
    while((pos = bs()) != -1) {
        size_type cols = block_array[pos].dim0();
        double * block_ptr = block_array[pos].begin();
        double * end = block_ptr+cols;
        /*create and start the timer */
        Timer time_it; time_it.start();
        //f(block_ptr,coef,step,cols,end);
        f(5.0,1.0,1.0,2000.0);
        time_it.end();
        std::cout << "duration " << time_it.duration() << std::endl;
    }
}
    

int main () {
    //level1_compute<neuromapp::cstandard> l1;
    //kernel_measure<neuromapp::cstandard>(l1);
    level3_compute<neuromapp::cstandard> l3;
    kernel_measure<neuromapp::cstandard> (l3);
}
