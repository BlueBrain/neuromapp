#include <iostream>
#include <chrono>
#include <iomanip>
#include <limits>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>

#define BOOST_TEST_MODULE devin_block_test
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

//local code for inclusion
#include "compression/allocator.h"
#include "compression/conv_info.h"
#include "compression/exception.h"
#include "compression/block.h"
#include "compression/bit_shifting.h"
#include "compression/timer_tool.h"
#include "compression/block_sort.h"
#include "neuromapp/compression/common/data/path.h"
using neuromapp::block;
using neuromapp::cstandard;
using namespace std;


//this is a vector of the filenames that hold csv data for testing

vector<string> solo_fnames {  "block_data/values_10_a8213trans_solo.csv", "block_data/values_10_a8214trans_solo.csv", "block_data/values_10_a8215trans_solo.csv", "block_data/values_10_a8216trans_solo.csv", "block_data/values_10_a8217trans_solo.csv", "block_data/values_10_a8218trans_solo.csv", "block_data/values_10_a8219trans_solo.csv", "block_data/values_10_a8220trans_solo.csv", "block_data/values_10_a8749trans_solo.csv", "block_data/values_10_a8750trans_solo.csv", "block_data/values_10_a8751trans_solo.csv", "block_data/values_10_a8752trans_solo.csv", "block_data/values_10_a8761trans_solo.csv", "block_data/values_8_a10249trans_solo.csv", "block_data/values_8_a10250trans_solo.csv", "block_data/values_8_a10251trans_solo.csv", "block_data/values_8_a10252trans_solo.csv", "block_data/values_8_a10256trans_solo.csv", "block_data/values_8_a10261trans_solo.csv", "block_data/values_8_a10262trans_solo.csv", "block_data/values_8_a10263trans_solo.csv", "block_data/values_8_a10264trans_solo.csv", "block_data/values_8_a8780trans_solo.csv", "block_data/values_8_a8781trans_solo.csv", "block_data/values_8_a8801trans_solo.csv", "block_data/values_8_a8802trans_solo.csv", "block_data/values_8_a8803trans_solo.csv", "block_data/values_8_a8804trans_solo.csv", "block_data/values_9_a10237trans_solo.csv", "block_data/values_9_a10238trans_solo.csv"};


vector<string> bulk_fnames { "block_data/values_10_a8213trans_bulk.csv", "block_data/values_10_a8214trans_bulk.csv", "block_data/values_10_a8215trans_bulk.csv", "block_data/values_10_a8216trans_bulk.csv", "block_data/values_10_a8217trans_bulk.csv", "block_data/values_10_a8218trans_bulk.csv", "block_data/values_10_a8219trans_bulk.csv", "block_data/values_10_a8220trans_bulk.csv", "block_data/values_10_a8749trans_bulk.csv", "block_data/values_10_a8750trans_bulk.csv", "block_data/values_10_a8751trans_bulk.csv", "block_data/values_10_a8752trans_bulk.csv", "block_data/values_10_a8761trans_bulk.csv", "block_data/values_8_a10249trans_bulk.csv", "block_data/values_8_a10250trans_bulk.csv", "block_data/values_8_a10251trans_bulk.csv", "block_data/values_8_a10252trans_bulk.csv", "block_data/values_8_a10256trans_bulk.csv", "block_data/values_8_a10261trans_bulk.csv", "block_data/values_8_a10262trans_bulk.csv", "block_data/values_8_a10263trans_bulk.csv", "block_data/values_8_a10264trans_bulk.csv",  "block_data/values_8_a8780trans_bulk.csv",  "block_data/values_8_a8781trans_bulk.csv",  "block_data/values_8_a8801trans_bulk.csv",  "block_data/values_8_a8802trans_bulk.csv",  "block_data/values_8_a8803trans_bulk.csv",  "block_data/values_8_a8804trans_bulk.csv",  "block_data/values_9_a10237trans_bulk.csv",  "block_data/values_9_a10238trans_bulk.csv"};

//holder struct for combos of numeric type and allocator policy
template <class T,class A>
struct shell {
    typedef T value_type;
    typedef A allocator_type;
};

//should I be testing the align this whole time?
typedef boost::mpl::list<shell<float, neuromapp::cstandard>,
                         shell<double, neuromapp::cstandard>,
                         shell<float, neuromapp::align>,
                         shell<double, neuromapp::align>> test_allocator_types;

//this function generates the correct format for the string which we compare the block output against
//generic def
template <typename T>
string create_correct_string(string original_str) {
    std::cout << "generic version called, check typing" << std::endl;
    return string("");
}
//probably a lot of tests...
BOOST_AUTO_TEST_CASE_TEMPLATE(compression_test,T,test_allocator_types) {
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    typedef typename Conv_info<value_type>::bytetype bin_type;
    vector<string> csv_type{"bulk","solo"};
    int type_ind = 0;
    for (vector<string> fname_container : {bulk_fnames,solo_fnames}) {
        for (string fname : fname_container) {
            chrono::time_point<chrono::system_clock> start,end;
            // make sure that the full path is correct
            ifstream ifile(mapp::path_specifier::give_path() + fname);

            // create a block using the read
            block<value_type,allocator_type> b1;
            //make a copy of b1
            ifile>>b1;
            block<bin_type,allocator_type> dec_block = neuromapp::generate_split_block<value_type,allocator_type>(b1);
            block<value_type, allocator_type> b2 = neuromapp::generate_unsplit_block<value_type,allocator_type>(dec_block);
            // compare the two blocks should be equal
            BOOST_CHECK(b1==b2);
        }
        type_ind++;
    }
}


