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

#define BOOST_TEST_MODULE block_copy_of_stream
/*this is the number of elements generated for filling the block*/
#define BLOCK_SIZE 8000
#define VECTOR_SIZE 15625
/*this is the number of times that we run each benchmark computation before taking the minimum time*/
#define NUM_ROUNDS 10
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

//local code for inclusion
#include "compression/allocator.h"
#include "compression/timer_tool.h"
#include "compression/exception.h"
#include "compression/block.h"
using neuromapp::block;
using neuromapp::cstandard;
using neuromapp::Timer;
using namespace std;
//holder struct for combos of numeric type and allocator policy
template <class T,class A>
struct shell {
    typedef T value_type;
    typedef A allocator_type;
};

//should I be testing the align this whole time?
typedef boost::mpl::list<shell<double, neuromapp::cstandard>> test_allocator_types;
//
//        shell<float, neuromapp::cstandard>,
//        shell<int, neuromapp::cstandard>,
//        shell<int, neuromapp::align>,
//        shell<float, neuromapp::align>,
//        shell<double, neuromapp::align>>
//        test_allocator_types;



//TODO make a test for the same total counts of eeach number, currently ther's a qustenio of whetehr the zeros get added in the malignblocks

BOOST_AUTO_TEST_CASE_TEMPLATE(stream_test,T,test_allocator_types){
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    typedef value_type * pointer;
    typedef pointer iterator;
    typedef size_t size_type;
    Timer time_it;
    //TODO look up pragma omp vector initialization
    vector<block<value_type,allocator_type>> v_a (VECTOR_SIZE);
    vector<block<value_type,allocator_type>> v_b (VECTOR_SIZE);
    for(block<value_type,allocator_type> &b : v_a) {
        b.resize(BLOCK_SIZE);
    }
    for(block<value_type,allocator_type> &b : v_b) {
        b.resize(BLOCK_SIZE);
    }
    //prepare for the copy operation
    double min_time;
    for (int round = 0; round < NUM_ROUNDS ; round++) {
        time_it.start();
        for (int i = 0; i < VECTOR_SIZE;i++) {
            block<value_type,allocator_type> & a = v_a[i];
            block<value_type,allocator_type> & b = v_b[i];
            pointer  ptr_a = a.data();
            pointer  ptr_b = b.data();
            for (int j=0; j < BLOCK_SIZE;j++) {
                ptr_a[j] = ptr_b[j];
            }

        }
        time_it.end();
        if (round == 0) min_time = time_it.duration();
        else if(min_time > time_it.duration()) min_time = time_it.duration();
    }
    std::cout << "minimum time for copy operation: " << min_time << " ms" << std::endl;
    
    double bandwith = VECTOR_SIZE*v_a[0].memory_allocated()*3*pow(10,-6) / (1000/min_time); // this will be in MB/s
    std::cout << "bandwith: " << bandwith << " MB/s" << std::endl;
}

