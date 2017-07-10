#include <iostream>
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
#define BLOCK_SIZE 16384000
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
typedef boost::mpl::list<shell<int, neuromapp::cstandard>,
        shell<float, neuromapp::cstandard>,
        shell<double, neuromapp::cstandard>,
        shell<int, neuromapp::align>,
        shell<float, neuromapp::align>,
        shell<double, neuromapp::align>>
        test_allocator_types;



        //TODO make a test for the same total counts of eeach number, currently ther's a qustenio of whetehr the zeros get added in the malignblocks

        BOOST_AUTO_TEST_CASE_TEMPLATE(stream_test,T,test_allocator_types){
            typedef typename T::value_type value_type;
            typedef typename T::allocator_type allocator_type;
            typedef value_type * pointer;
            typedef pointer iterator;
            typedef size_t size_type;
            size_type total_elements = BLOCK_SIZE;
            Timer time_it;
            block<value_type,allocator_type> b1(total_elements),b2(total_elements),b3(total_elements);  
            //vector<block<value_type,allocator_type>> block_container {b1,b2,b3};
            vector<block<value_type,allocator_type>> block_container {b1,b2};
            std::cout << "reading into blocks" << std::endl;
            time_it.start();
            for (block<value_type,allocator_type> b_iter: block_container){
                ifstream ifile;
                ifile.open("STREAM_data/randomnums.csv");
                ifile >> b_iter;
            }
            time_it.end();
            std::cout << "reading took: " << time_it.duration() << "(ms)" << std::endl;
            //run the copy benchmark, will need to be done certain number of times
            std::cout << "starting copy computation" << std::endl;
            for(int round = 0; round < NUM_ROUNDS;round++) {
                size_type i = 0;
                time_it.start();
                while (i < total_elements) {
                    b1(i,0) = b2(i,0);
                    i++;
                }
                time_it.end();
                std::cout << "copy took: " <<time_it.duration() <<"(ms)" << std::endl;
            }
        }

