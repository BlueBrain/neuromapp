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
/*comment out the compress line to enable/disable the compression options*/
//#define COMPRESS
#define BOOST_TEST_MODULE block_copy_of_stream
/*this is the number of elements generated for filling the block*/
#define BLOCK_SIZE 8000
#define VECTOR_SIZE 64
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
    vector<block<value_type,allocator_type>> v_a ;
    vector<block<value_type,allocator_type>> v_b ;
    vector<block<value_type,allocator_type>> v_c ;
#if defined(COMPRESS)
    std::cout << "using compress" << std::endl;
#else
    std::cout << "no compress" << std::endl;
#endif
    for(int i = 0; i < VECTOR_SIZE;i++) {
        block<value_type,allocator_type> ba(BLOCK_SIZE);
        block<value_type,allocator_type> bb(BLOCK_SIZE);
        block<value_type,allocator_type> bc(BLOCK_SIZE);
#if defined(COMPRESS)
        ba.compress();
        bb.compress();
        bc.compress();
#endif
        v_a.push_back(ba);
        v_b.push_back(bb);
        v_c.push_back(bc);
    }
    double mem_used = v_a[0].memory_allocated()*3*pow(10,-6);
    //prepare for the copy operation
    double min_time;
    for (int round = 0; round < NUM_ROUNDS ; round++) {
#if defined(COMPRESS)
            v_a[i].uncompress();
            v_b[i].uncompress();
#endif
        for (int i = 0; i < VECTOR_SIZE;i++) {
            time_it.start();
            block<value_type,allocator_type> & a = v_a[i];
            block<value_type,allocator_type> & b = v_b[i];
            pointer  ptr_a = a.data();
            pointer  ptr_b = b.data();
            for (int j=0; j < BLOCK_SIZE;j++) {
                ptr_a[j] = ptr_b[j];
            }
            time_it.end();
            if (round == 0) min_time = time_it.duration();
            else if(min_time > time_it.duration()) min_time = time_it.duration();
        }
#if defined(COMPRESS)
            v_a[i].compress();
            v_b[i].compress();
#endif
    }

    double copy_bandwith =  mem_used *(1000/min_time) ; // this will be in MBs
    //prepare for the add operation
    for (int round = 0; round < NUM_ROUNDS ; round++) {
#if defined(COMPRESS)
            v_a[i].uncompress();
            v_b[i].uncompress();
            v_c[i].uncompress();
#endif
        for (int i = 0; i < VECTOR_SIZE;i++) {
            time_it.start();
            block<value_type,allocator_type> & a = v_a[i];
            block<value_type,allocator_type> & b = v_b[i];
            block<value_type,allocator_type> & c = v_c[i];
            pointer  ptr_a = a.data();
            pointer  ptr_b = b.data();
            pointer  ptr_c = c.data();
            for (int j=0; j < BLOCK_SIZE;j++) {
                ptr_a[j] = ptr_b[j] + ptr_c[j];
            }
            time_it.end();
            if (round == 0) min_time = time_it.duration();
            else if(min_time > time_it.duration()) min_time = time_it.duration();
        }
#if defined(COMPRESS)
            v_a[i].compress();
            v_b[i].compress();
            v_c[i].compress();
#endif
    }
    mem_used = v_a[0].memory_allocated()*2*pow(10,-6);
    double add_bandwith = mem_used*(1000/min_time) ; // this will be in MBs

    //scale operation
    for (int round = 0; round < NUM_ROUNDS ; round++) {
#if defined(COMPRESS)
            time_it.start();
            v_a[i].uncompress();
            v_b[i].uncompress();
#endif
        for (int i = 0; i < VECTOR_SIZE;i++) {
            block<value_type,allocator_type> & a = v_a[i];
            block<value_type,allocator_type> & b = v_b[i];
            pointer  ptr_a = a.data();
            pointer  ptr_b = b.data();
            value_type scale = 5;
            for (int j=0; j < BLOCK_SIZE;j++) {
                ptr_a[j] = scale*ptr_b[j];
            }
            time_it.end();
            if (round == 0) min_time = time_it.duration();
            else if(min_time > time_it.duration()) min_time = time_it.duration();
        }
#if defined(COMPRESS)
            v_a[i].compress();
            v_b[i].compress();
#endif
    }
    double scale_bandwith = mem_used*(1000/min_time) ; // this will be in MBs

    //triad operation
    for (int round = 0; round < NUM_ROUNDS ; round++) {
#if defined(COMPRESS)
            v_a[i].uncompress();
            v_b[i].uncompress();
            v_c[i].uncompress();
#endif
        for (int i = 0; i < VECTOR_SIZE;i++) {
            time_it.start();
            block<value_type,allocator_type> & a = v_a[i];
            block<value_type,allocator_type> & b = v_b[i];
            block<value_type,allocator_type> & c = v_c[i];
            pointer  ptr_a = a.data();
            pointer  ptr_b = b.data();
            pointer  ptr_c = c.data();
            value_type scale = 5;
            for (int j=0; j < BLOCK_SIZE;j++) {
                ptr_a[j] = scale*ptr_b[j] + ptr_c[j];
            }
            time_it.end();
            if (round == 0) min_time = time_it.duration();
            else if(min_time > time_it.duration()) min_time = time_it.duration();
        }
#if defined(COMPRESS)
            v_a[i].compress();
            v_b[i].compress();
            v_c[i].compress();
#endif
    }
    double triad_bandwith = mem_used*(1000/min_time) ; // this will be in MBs
    //print message
    std::cout << left;
    std::cout << setw(20) <<  "operation: copy " <<setw(13) << "bandwith : " <<setw(16) <<   setprecision(5) << copy_bandwith << setw(5) << "MBs" << std::endl;
    std::cout<< setw(20) << "operation: add " << setw(13) << "bandwith : " <<setw(16) <<   setprecision(5) << add_bandwith << setw(5) << "MBs" << std::endl;
    std::cout<< setw(20) << "operation: scale " << setw(13) << "bandwith : " <<setw(16) << setprecision(5) << scale_bandwith << setw(5) << "MBs" << std::endl;
    std::cout<< setw(20) << "operation: triad " << setw(13) << "bandwith : " <<setw(16) << setprecision(5) << triad_bandwith << setw(5) << "MBs" << std::endl;
}

