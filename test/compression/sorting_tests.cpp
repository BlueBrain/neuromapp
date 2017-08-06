
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

#define BOOST_TEST_MODULE devin_block_test
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

//local code for inclusion
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block.h"
#include "compression/block_sort.h"
#include "neuromapp/compression/common/data/path.h"
using neuromapp::block;
using neuromapp::cstandard;
using neuromapp::col_sort;
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


vector<string> practice_files ={"trans_data/visually_easy.csv"};

        vector<string> solo_files = {"block_data/values_10_a8214trans_solo.csv", "block_data/values_10_a8215trans_solo.csv", "block_data/values_10_a8216trans_solo.csv", "block_data/values_10_a8217trans_solo.csv", "block_data/values_10_a8218trans_solo.csv", "block_data/values_10_a8219trans_solo.csv", "block_data/values_10_a8220trans_solo.csv", "block_data/values_10_a8749trans_solo.csv", "block_data/values_10_a8750trans_solo.csv", "block_data/values_10_a8751trans_solo.csv", "block_data/values_10_a8752trans_solo.csv", "block_data/values_10_a8761trans_solo.csv", "block_data/values_8_a10249trans_solo.csv", "block_data/values_8_a10250trans_solo.csv", "block_data/values_8_a10251trans_solo.csv", "block_data/values_8_a10252trans_solo.csv", "block_data/values_8_a10256trans_solo.csv", "block_data/values_8_a10261trans_solo.csv", "block_data/values_8_a10262trans_solo.csv", "block_data/values_8_a10263trans_solo.csv", "block_data/values_8_a10264trans_solo.csv", "block_data/values_8_a8780trans_solo.csv", "block_data/values_8_a8781trans_solo.csv", "block_data/values_8_a8801trans_solo.csv", "block_data/values_8_a8802trans_solo.csv", "block_data/values_8_a8803trans_solo.csv", "block_data/values_8_a8804trans_solo.csv", "block_data/values_9_a10237trans_solo.csv", "block_data/values_9_a10238trans_solo.csv", "block_data/values_9_a10239trans_solo.csv", "block_data/values_9_a10240trans_solo.csv", "block_data/values_9_a10245trans_solo.csv", "block_data/values_9_a10257trans_solo.csv", "block_data/values_9_a10258trans_solo.csv", "block_data/values_9_a10259trans_solo.csv", "block_data/values_9_a10260trans_solo.csv", "block_data/values_9_a513trans_solo.csv", "block_data/values_9_a514trans_solo.csv", "block_data/values_9_a515trans_solo.csv", "block_data/values_9_a516trans_solo.csv", "block_data/values_9_a8737trans_solo.csv", "block_data/values_9_a8738trans_solo.csv", "block_data/values_9_a8739trans_solo.csv", "block_data/values_9_a8740trans_solo.csv", "block_data/values_9_a8782trans_solo.csv", "block_data/values_9_a8783trans_solo.csv", "block_data/values_9_a8784trans_solo.csv", "block_data/values_9_a8785trans_solo.csv", "block_data/values_9_a8786trans_solo.csv", "block_data/values_9_a8787trans_solo.csv", "block_data/values_9_a8788trans_solo.csv", "block_data/values_9_a8789trans_solo.csv", "block_data/values_9_a8790trans_solo.csv", "block_data/values_9_a8791trans_solo.csv", "block_data/values_9_a8792trans_solo.csv", "block_data/values_9_a8825trans_solo.csv", "block_data/values_9_a8826trans_solo.csv", "block_data/values_9_a8827trans_solo.csv", "block_data/values_9_a8828trans_solo.csv"};

vector<string>bulk_files ={"block_data/values_9_a8828trans_bulk.csv","block_data/values_9_a8827trans_bulk.csv","block_data/values_9_a8826trans_bulk.csv","block_data/values_9_a8825trans_bulk.csv","block_data/values_9_a8792trans_bulk.csv","block_data/values_9_a8791trans_bulk.csv","block_data/values_9_a8790trans_bulk.csv","block_data/values_9_a8789trans_bulk.csv","block_data/values_9_a8788trans_bulk.csv","block_data/values_9_a8787trans_bulk.csv","block_data/values_9_a8786trans_bulk.csv","block_data/values_9_a8785trans_bulk.csv","block_data/values_9_a8784trans_bulk.csv","block_data/values_9_a8783trans_bulk.csv","block_data/values_9_a8782trans_bulk.csv","block_data/values_9_a8740trans_bulk.csv","block_data/values_9_a8739trans_bulk.csv","block_data/values_9_a8738trans_bulk.csv","block_data/values_9_a8737trans_bulk.csv","block_data/values_9_a516trans_bulk.csv","block_data/values_9_a515trans_bulk.csv","block_data/values_9_a514trans_bulk.csv","block_data/values_9_a513trans_bulk.csv","block_data/values_9_a10260trans_bulk.csv","block_data/values_9_a10259trans_bulk.csv","block_data/values_9_a10258trans_bulk.csv","block_data/values_9_a10257trans_bulk.csv","block_data/values_9_a10245trans_bulk.csv","block_data/values_9_a10240trans_bulk.csv","block_data/values_9_a10239trans_bulk.csv","block_data/values_9_a10238trans_bulk.csv","block_data/values_9_a10237trans_bulk.csv","block_data/values_8_a8804trans_bulk.csv","block_data/values_8_a8803trans_bulk.csv","block_data/values_8_a8802trans_bulk.csv","block_data/values_8_a8801trans_bulk.csv","block_data/values_8_a8781trans_bulk.csv","block_data/values_8_a8780trans_bulk.csv","block_data/values_8_a10264trans_bulk.csv","block_data/values_8_a10263trans_bulk.csv","block_data/values_8_a10262trans_bulk.csv","block_data/values_8_a10261trans_bulk.csv","block_data/values_8_a10256trans_bulk.csv","block_data/values_8_a10252trans_bulk.csv","block_data/values_8_a10251trans_bulk.csv","block_data/values_8_a10250trans_bulk.csv","block_data/values_8_a10249trans_bulk.csv","block_data/values_10_a8761trans_bulk.csv","block_data/values_10_a8752trans_bulk.csv","block_data/values_10_a8751trans_bulk.csv","block_data/values_10_a8750trans_bulk.csv","block_data/values_10_a8749trans_bulk.csv","block_data/values_10_a8220trans_bulk.csv","block_data/values_10_a8219trans_bulk.csv","block_data/values_10_a8218trans_bulk.csv","block_data/values_10_a8217trans_bulk.csv","block_data/values_10_a8216trans_bulk.csv","block_data/values_10_a8215trans_bulk.csv","block_data/values_10_a8214trans_bulk.csv","block_data/values_10_a8213trans_bulk.csv"};

template <typename IT,typename size_type>
bool increasing_check(IT rhs,size_type cols_) {
// must go one less than the number of columns because we are comparing to the following
	for(size_type i = 0; i < cols_ -1;i++) {
        if (*rhs > *(rhs+1)) {
            std::cout << *rhs << " was bigger than " << *(rhs+1) << std::endl;
            return false;
        }
        rhs++;
    }
    return true;
}

/* _s stands for s, _ns not sorted */
template<typename V,typename A,typename size_type>
bool full_col_persistence( block<V,A> & sorted_block, block<V,A> & unsorted_block, size_type& sort_row) {
    size_type cols_ = sorted_block.dim0();
    size_type rows_ = sorted_block.num_rows();
    vector<V> sort_col_sums,unsorted_col_sums;
    for (size_type i = 0 ;i < cols_ ; i++) {
        V unsorted_col = 0,sorted_col = 0;
        for (size_type j = 0;j < rows_ ; j++) {
            unsorted_col += unsorted_block(i,j);
            sorted_col += sorted_block(i,j);
        }
        sort_col_sums.push_back(sorted_col);
        unsorted_col_sums.push_back(unsorted_col);
    }
    // search for each value in sort_col_sums in the unsorted_col_sums, if something is left then we messed up a columns structure
    for (V sort_sum : sort_col_sums) {
        auto found = std::find(unsorted_col_sums.begin(),unsorted_col_sums.end(),sort_sum);
        if (found == unsorted_col_sums.end()) return false;
    }
    return true;
}



//TODO make a test for the same total counts of eeach number, currently ther's a qustenio of whetehr the zeros get added in the malignblocks

BOOST_AUTO_TEST_CASE_TEMPLATE(sort_test,T,test_allocator_types){
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    typedef value_type * pointer;
    typedef pointer iterator;
    typedef size_t size_type;
    for (vector<string> fname_container : {bulk_files,solo_files}) {
        for (string fname : fname_container) {
            std::cout << "fname is " <<fname << std::endl;
            // make sure the correct full path is given
            ifstream ifile(mapp::path_specifier::give_path() + fname);
            block<value_type,allocator_type> b1;
            ifile>> b1;
            size_type cols_ = b1.dim0();
            size_type sort_row = 0, other_row = 2;//TODO remove the references to the otehr row, might not need
            block<value_type,allocator_type> b2 = b1;
            col_sort(&b1,sort_row);
            //st stands for start
            iterator st_srow_s  = &b1(0,sort_row);
            iterator end_srow_s = &b1(cols_,sort_row);
            BOOST_CHECK_MESSAGE(true == increasing_check(st_srow_s,cols_),
                    "row wasn't strictly increasing\n");

            ; 
            BOOST_CHECK_MESSAGE(true == full_col_persistence(b1,b2,sort_row),
                    "value in row below wasn't consistent\n");

        }
    }
}

