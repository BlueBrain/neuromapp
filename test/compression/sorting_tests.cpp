
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
typedef boost::mpl::list<shell<int, neuromapp::cstandard>,
        shell<float, neuromapp::cstandard>,
        shell<double, neuromapp::cstandard>,
        shell<int, neuromapp::align>,
        shell<float, neuromapp::align>,
        shell<double, neuromapp::align>>
        test_allocator_types;


vector<string> practice_files ={"trans_data/visually_easy.csv","trans_data/ez_vis2.csv","trans_data/no_zeros_1st_row.csv"};

        vector<string> solo_files = {"trans_data/values_10_a8214trans_solo.csv", "trans_data/values_10_a8215trans_solo.csv", "trans_data/values_10_a8216trans_solo.csv", "trans_data/values_10_a8217trans_solo.csv", "trans_data/values_10_a8218trans_solo.csv", "trans_data/values_10_a8219trans_solo.csv", "trans_data/values_10_a8220trans_solo.csv", "trans_data/values_10_a8749trans_solo.csv", "trans_data/values_10_a8750trans_solo.csv", "trans_data/values_10_a8751trans_solo.csv", "trans_data/values_10_a8752trans_solo.csv", "trans_data/values_10_a8761trans_solo.csv", "trans_data/values_8_a10249trans_solo.csv", "trans_data/values_8_a10250trans_solo.csv", "trans_data/values_8_a10251trans_solo.csv", "trans_data/values_8_a10252trans_solo.csv", "trans_data/values_8_a10256trans_solo.csv", "trans_data/values_8_a10261trans_solo.csv", "trans_data/values_8_a10262trans_solo.csv", "trans_data/values_8_a10263trans_solo.csv", "trans_data/values_8_a10264trans_solo.csv", "trans_data/values_8_a8780trans_solo.csv", "trans_data/values_8_a8781trans_solo.csv", "trans_data/values_8_a8801trans_solo.csv", "trans_data/values_8_a8802trans_solo.csv", "trans_data/values_8_a8803trans_solo.csv", "trans_data/values_8_a8804trans_solo.csv", "trans_data/values_9_a10237trans_solo.csv", "trans_data/values_9_a10238trans_solo.csv", "trans_data/values_9_a10239trans_solo.csv", "trans_data/values_9_a10240trans_solo.csv", "trans_data/values_9_a10245trans_solo.csv", "trans_data/values_9_a10257trans_solo.csv", "trans_data/values_9_a10258trans_solo.csv", "trans_data/values_9_a10259trans_solo.csv", "trans_data/values_9_a10260trans_solo.csv", "trans_data/values_9_a513trans_solo.csv", "trans_data/values_9_a514trans_solo.csv", "trans_data/values_9_a515trans_solo.csv", "trans_data/values_9_a516trans_solo.csv", "trans_data/values_9_a8737trans_solo.csv", "trans_data/values_9_a8738trans_solo.csv", "trans_data/values_9_a8739trans_solo.csv", "trans_data/values_9_a8740trans_solo.csv", "trans_data/values_9_a8782trans_solo.csv", "trans_data/values_9_a8783trans_solo.csv", "trans_data/values_9_a8784trans_solo.csv", "trans_data/values_9_a8785trans_solo.csv", "trans_data/values_9_a8786trans_solo.csv", "trans_data/values_9_a8787trans_solo.csv", "trans_data/values_9_a8788trans_solo.csv", "trans_data/values_9_a8789trans_solo.csv", "trans_data/values_9_a8790trans_solo.csv", "trans_data/values_9_a8791trans_solo.csv", "trans_data/values_9_a8792trans_solo.csv", "trans_data/values_9_a8825trans_solo.csv", "trans_data/values_9_a8826trans_solo.csv", "trans_data/values_9_a8827trans_solo.csv", "trans_data/values_9_a8828trans_solo.csv"};

vector<string>bulk_files ={"trans_data/even_easier.csv","trans_data/values_9_a8828trans_bulk.csv","trans_data/values_9_a8827trans_bulk.csv","trans_data/values_9_a8826trans_bulk.csv","trans_data/values_9_a8825trans_bulk.csv","trans_data/values_9_a8792trans_bulk.csv","trans_data/values_9_a8791trans_bulk.csv","trans_data/values_9_a8790trans_bulk.csv","trans_data/values_9_a8789trans_bulk.csv","trans_data/values_9_a8788trans_bulk.csv","trans_data/values_9_a8787trans_bulk.csv","trans_data/values_9_a8786trans_bulk.csv","trans_data/values_9_a8785trans_bulk.csv","trans_data/values_9_a8784trans_bulk.csv","trans_data/values_9_a8783trans_bulk.csv","trans_data/values_9_a8782trans_bulk.csv","trans_data/values_9_a8740trans_bulk.csv","trans_data/values_9_a8739trans_bulk.csv","trans_data/values_9_a8738trans_bulk.csv","trans_data/values_9_a8737trans_bulk.csv","trans_data/values_9_a516trans_bulk.csv","trans_data/values_9_a515trans_bulk.csv","trans_data/values_9_a514trans_bulk.csv","trans_data/values_9_a513trans_bulk.csv","trans_data/values_9_a10260trans_bulk.csv","trans_data/values_9_a10259trans_bulk.csv","trans_data/values_9_a10258trans_bulk.csv","trans_data/values_9_a10257trans_bulk.csv","trans_data/values_9_a10245trans_bulk.csv","trans_data/values_9_a10240trans_bulk.csv","trans_data/values_9_a10239trans_bulk.csv","trans_data/values_9_a10238trans_bulk.csv","trans_data/values_9_a10237trans_bulk.csv","trans_data/values_8_a8804trans_bulk.csv","trans_data/values_8_a8803trans_bulk.csv","trans_data/values_8_a8802trans_bulk.csv","trans_data/values_8_a8801trans_bulk.csv","trans_data/values_8_a8781trans_bulk.csv","trans_data/values_8_a8780trans_bulk.csv","trans_data/values_8_a10264trans_bulk.csv","trans_data/values_8_a10263trans_bulk.csv","trans_data/values_8_a10262trans_bulk.csv","trans_data/values_8_a10261trans_bulk.csv","trans_data/values_8_a10256trans_bulk.csv","trans_data/values_8_a10252trans_bulk.csv","trans_data/values_8_a10251trans_bulk.csv","trans_data/values_8_a10250trans_bulk.csv","trans_data/values_8_a10249trans_bulk.csv","trans_data/values_10_a8761trans_bulk.csv","trans_data/values_10_a8752trans_bulk.csv","trans_data/values_10_a8751trans_bulk.csv","trans_data/values_10_a8750trans_bulk.csv","trans_data/values_10_a8749trans_bulk.csv","trans_data/values_10_a8220trans_bulk.csv","trans_data/values_10_a8219trans_bulk.csv","trans_data/values_10_a8218trans_bulk.csv","trans_data/values_10_a8217trans_bulk.csv","trans_data/values_10_a8216trans_bulk.csv","trans_data/values_10_a8215trans_bulk.csv","trans_data/values_10_a8214trans_bulk.csv","trans_data/values_10_a8213trans_bulk.csv"};

template <typename IT,typename sz>
bool increasing_check(IT rhs,sz cols_) {
// must go one less than the number of columns because we are comparing to the following
	for(sz i = 0; i < cols_ -1;i++) {
        if (*rhs > *(rhs+1)) {
            std::cout << *rhs << " was bigger than " << *(rhs+1) << std::endl;
            return false;
        }
        rhs++;
    }
    return true;
}

/* _s stands for s, _ns not sorted */
template<typename V,typename A,typename sz>
int full_col_persistence( block<V,A> & sorted_block, block<V,A> & unsorted_block, sz& sort_row) {
    int mismatches = 0;
    sz cols_ = sorted_block.dim0();
    sz rows_ = sorted_block.num_rows();
    std::vector<sz> ref_vector(cols_);// second argument is how many places to start with
    //use iota to fill 
    std::iota (ref_vector.begin(),ref_vector.end(),0);
    //sort ref_vector values based on the sort_row in the blocks, using the unsorted columns as the compare criteria (mimic the actual col_sort)
    std::sort (ref_vector.begin(),ref_vector.end(), [&unsorted_block,&sort_row] ( sz & lhs_ind, sz & rhs_ind) {
            return unsorted_block(lhs_ind,sort_row) < unsorted_block(rhs_ind,sort_row) ? true: false;
            });
    std::copy(ref_vector.begin(),ref_vector.end(),std::ostream_iterator<sz>(std::cout," "));
    std::cout << "" << std::endl;
    std::cout << "sorted" <<std::endl <<  sorted_block << std::endl;
    std::cout << "unsorted" << std::endl << unsorted_block << std::endl;
    for (size_t i = 0 ;i < cols_ ; i++) {
        //check that all positions within the column are the same in both blocks
        for (size_t j = 0;j < rows_ ; j++) {
            std::cout << "col " << i << " row " << j << "...";
            std::cout << sorted_block(i,j) << "VS" << unsorted_block(ref_vector[i],j) << std::endl;
            //if(sorted_block(i,j) != unsorted_block(ref_vector[i],j)) mismatches++;
            //std::cout <<setw(10) << "||" << "sorted: " << sorted_block(i,j) << " unsorted: " << unsorted_block(ref_vector[i],j) << " mismatches " << mismatches << "||" ;
            //if ( j %3 == 0) std::cout << std::endl;
        }

    }
    return mismatches;
}



//TODO make a test for the same total counts of eeach number, currently ther's a qustenio of whetehr the zeros get added in the malignblocks

BOOST_AUTO_TEST_CASE_TEMPLATE(sort_test,T,test_allocator_types){
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    typedef value_type * pointer;
    typedef pointer iterator;
    for (vector<string> fname_container : {practice_files}) {
        for (string fname : fname_container) {
            std::cout << "fname is " <<fname << std::endl;
            ifstream ifile(fname);
            block<value_type,allocator_type> b1;
            ifile>> b1;
            size_t cols_ = b1.dim0();
            size_t sort_row = 0, other_row = 2;//TODO remove the references to the otehr row, might not need
            block<value_type,allocator_type> b2(b1);
            b1.col_sort(sort_row);
            //st stands for start
            iterator st_srow_s  = &b1(0,sort_row);
            iterator end_srow_s = &b1(cols_,sort_row);
            BOOST_CHECK_MESSAGE(true == increasing_check(st_srow_s,cols_),
                    "row wasn't strictly increasing\n");

            int mismatch = full_col_persistence(b1,b2,sort_row); 
            BOOST_CHECK_MESSAGE(0 == mismatch,
                    "value in row below wasn't consistent\n" + std::to_string(mismatch));

        }
    }
}

