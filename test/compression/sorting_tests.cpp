
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
#include "compression/exception.h"
#include "compression/block.h"
#include "compression/block_sort.h"
using neuromapp::block;
using neuromapp::Sorter;
using neuromapp::cstandard;
using namespace std;
//holder struct for combos of numeric type and allocator policy
template <class T,class A>
struct shell {
    typedef T value_type;
    typedef A allocator_type;
};

//should I be testing the align this whole time?
typedef boost::mpl::list<shell<int, neuromapp::align>,
                        shell<float, neuromapp::align>,
                         shell<double, neuromapp::align>,
                         shell<int, neuromapp::cstandard>,
                        shell<float, neuromapp::cstandard>,
                         shell<double, neuromapp::cstandard>>
    test_allocator_types;


vector<string> solo_files = {"trans_data/values_10_a8214trans_solo.csv", "trans_data/values_10_a8215trans_solo.csv", "trans_data/values_10_a8216trans_solo.csv", "trans_data/values_10_a8217trans_solo.csv", "trans_data/values_10_a8218trans_solo.csv", "trans_data/values_10_a8219trans_solo.csv", "trans_data/values_10_a8220trans_solo.csv", "trans_data/values_10_a8749trans_solo.csv", "trans_data/values_10_a8750trans_solo.csv", "trans_data/values_10_a8751trans_solo.csv", "trans_data/values_10_a8752trans_solo.csv", "trans_data/values_10_a8761trans_solo.csv", "trans_data/values_8_a10249trans_solo.csv", "trans_data/values_8_a10250trans_solo.csv", "trans_data/values_8_a10251trans_solo.csv", "trans_data/values_8_a10252trans_solo.csv", "trans_data/values_8_a10256trans_solo.csv", "trans_data/values_8_a10261trans_solo.csv", "trans_data/values_8_a10262trans_solo.csv", "trans_data/values_8_a10263trans_solo.csv", "trans_data/values_8_a10264trans_solo.csv", "trans_data/values_8_a8780trans_solo.csv", "trans_data/values_8_a8781trans_solo.csv", "trans_data/values_8_a8801trans_solo.csv", "trans_data/values_8_a8802trans_solo.csv", "trans_data/values_8_a8803trans_solo.csv", "trans_data/values_8_a8804trans_solo.csv", "trans_data/values_9_a10237trans_solo.csv", "trans_data/values_9_a10238trans_solo.csv", "trans_data/values_9_a10239trans_solo.csv", "trans_data/values_9_a10240trans_solo.csv", "trans_data/values_9_a10245trans_solo.csv", "trans_data/values_9_a10257trans_solo.csv", "trans_data/values_9_a10258trans_solo.csv", "trans_data/values_9_a10259trans_solo.csv", "trans_data/values_9_a10260trans_solo.csv", "trans_data/values_9_a513trans_solo.csv", "trans_data/values_9_a514trans_solo.csv", "trans_data/values_9_a515trans_solo.csv", "trans_data/values_9_a516trans_solo.csv", "trans_data/values_9_a8737trans_solo.csv", "trans_data/values_9_a8738trans_solo.csv", "trans_data/values_9_a8739trans_solo.csv", "trans_data/values_9_a8740trans_solo.csv", "trans_data/values_9_a8782trans_solo.csv", "trans_data/values_9_a8783trans_solo.csv", "trans_data/values_9_a8784trans_solo.csv", "trans_data/values_9_a8785trans_solo.csv", "trans_data/values_9_a8786trans_solo.csv", "trans_data/values_9_a8787trans_solo.csv", "trans_data/values_9_a8788trans_solo.csv", "trans_data/values_9_a8789trans_solo.csv", "trans_data/values_9_a8790trans_solo.csv", "trans_data/values_9_a8791trans_solo.csv", "trans_data/values_9_a8792trans_solo.csv", "trans_data/values_9_a8825trans_solo.csv", "trans_data/values_9_a8826trans_solo.csv", "trans_data/values_9_a8827trans_solo.csv", "trans_data/values_9_a8828trans_solo.csv"};

vector<string>bulk_files ={"trans_data/values_9_a8828trans_bulk.csv","trans_data/values_9_a8827trans_bulk.csv","trans_data/values_9_a8826trans_bulk.csv","trans_data/values_9_a8825trans_bulk.csv","trans_data/values_9_a8792trans_bulk.csv","trans_data/values_9_a8791trans_bulk.csv","trans_data/values_9_a8790trans_bulk.csv","trans_data/values_9_a8789trans_bulk.csv","trans_data/values_9_a8788trans_bulk.csv","trans_data/values_9_a8787trans_bulk.csv","trans_data/values_9_a8786trans_bulk.csv","trans_data/values_9_a8785trans_bulk.csv","trans_data/values_9_a8784trans_bulk.csv","trans_data/values_9_a8783trans_bulk.csv","trans_data/values_9_a8782trans_bulk.csv","trans_data/values_9_a8740trans_bulk.csv","trans_data/values_9_a8739trans_bulk.csv","trans_data/values_9_a8738trans_bulk.csv","trans_data/values_9_a8737trans_bulk.csv","trans_data/values_9_a516trans_bulk.csv","trans_data/values_9_a515trans_bulk.csv","trans_data/values_9_a514trans_bulk.csv","trans_data/values_9_a513trans_bulk.csv","trans_data/values_9_a10260trans_bulk.csv","trans_data/values_9_a10259trans_bulk.csv","trans_data/values_9_a10258trans_bulk.csv","trans_data/values_9_a10257trans_bulk.csv","trans_data/values_9_a10245trans_bulk.csv","trans_data/values_9_a10240trans_bulk.csv","trans_data/values_9_a10239trans_bulk.csv","trans_data/values_9_a10238trans_bulk.csv","trans_data/values_9_a10237trans_bulk.csv","trans_data/values_8_a8804trans_bulk.csv","trans_data/values_8_a8803trans_bulk.csv","trans_data/values_8_a8802trans_bulk.csv","trans_data/values_8_a8801trans_bulk.csv","trans_data/values_8_a8781trans_bulk.csv","trans_data/values_8_a8780trans_bulk.csv","trans_data/values_8_a10264trans_bulk.csv","trans_data/values_8_a10263trans_bulk.csv","trans_data/values_8_a10262trans_bulk.csv","trans_data/values_8_a10261trans_bulk.csv","trans_data/values_8_a10256trans_bulk.csv","trans_data/values_8_a10252trans_bulk.csv","trans_data/values_8_a10251trans_bulk.csv","trans_data/values_8_a10250trans_bulk.csv","trans_data/values_8_a10249trans_bulk.csv","trans_data/values_10_a8761trans_bulk.csv","trans_data/values_10_a8752trans_bulk.csv","trans_data/values_10_a8751trans_bulk.csv","trans_data/values_10_a8750trans_bulk.csv","trans_data/values_10_a8749trans_bulk.csv","trans_data/values_10_a8220trans_bulk.csv","trans_data/values_10_a8219trans_bulk.csv","trans_data/values_10_a8218trans_bulk.csv","trans_data/values_10_a8217trans_bulk.csv","trans_data/values_10_a8216trans_bulk.csv","trans_data/values_10_a8215trans_bulk.csv","trans_data/values_10_a8214trans_bulk.csv","trans_data/values_10_a8213trans_bulk.csv"};

template <typename IT>
bool increasing_check(IT rhs,IT lhs) {
    while (rhs +1 != lhs) {
        if (*rhs > *(rhs+1)) return false;
        rhs++;
    }
    return true;
}

/* _s stands for s, _ns not sorted */
template<typename IT,typename sz>
int full_col_persistence(IT r1_s,IT r2_s,IT r1_ns,IT r2_ns,sz cols_to_compare) {
    int mismatch_count = 0;
    while(cols_to_compare > 0) {
        IT&& r1_temp_cpy  {std::move(r1_s)};
        IT&& r2_temp_cpy  {std::move(r2_s)};
        while(*r1_temp_cpy != *r1_ns) {
            std::cout << "r1 unsorted:" << * r1_ns << "  r1_sorted: " << *r1_temp_cpy  << std::endl;
            r1_temp_cpy++;
            r2_temp_cpy++;
        }
        std::cout << "in other row r2s is " <<*r2_temp_cpy << " VS " << *r2_ns << std::endl;
        if (*r2_temp_cpy != *r2_ns) mismatch_count++;
        r1_s++;
        r2_s++;
        r1_ns++;
        r2_ns++;
        cols_to_compare--;
    }
    return mismatch_count;
}




BOOST_AUTO_TEST_CASE_TEMPLATE(sort_test,T,test_allocator_types){
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    typedef value_type * pointer;
    typedef pointer iterator;
    for (vector<string> fname_container : {bulk_files}) {
        for (string fname : fname_container) {
            std::cout << "fname is " <<fname << std::endl;
            ifstream ifile(fname);
            block<value_type,allocator_type> b1;
            ifile>> b1;
            size_t cols_ = b1.num_cols();
            std::cout << "cols_ is  at test" << cols_ << std::endl;
            size_t sort_row = 3, other_row = 2;
            block<value_type,allocator_type> b2(b1);
            b1.col_sort(sort_row);
            //st stands for stand
            iterator st_srow_s  = &b1(0,sort_row);
            iterator end_srow_s = &b1(cols_,sort_row);
            b1.print_row(other_row,"b1");
            BOOST_CHECK_MESSAGE(true == increasing_check(st_srow_s,end_srow_s),
                    "row wasn't strictly increasing\n");
            
            iterator st_srow_ns = &b2(0,sort_row);
            //3rd with 0 indexing
            iterator st_other_ns = &b2(0,other_row);
            iterator st_other_s = &b1(0,other_row);
            size_t count_up{0};
            while (count_up++< cols_) {
                std::cout << "," << *(st_other_s++);
            }



            //BOOST_CHECK_MESSAGE(0 == full_col_persistence(st_srow_s,st_other_s,st_srow_ns,st_other_ns,cols_),
                    //"value in row below wasn't consistent");

        }
    }
}

