#ifndef BLOCK_SORT_H
#define BLOCK_SORT_H
#include <iostream>
#include <string>
#include <algorithm>
#include "block.h"
//TODO remove std namespace
//

using namespace std;

namespace neuromapp {
    // create standard block sort,
    // and specialized versions for 1 dim and 2 dims
    typedef std::size_t size_type;
    template<typename V,typename A>
    void col_sort (block<V,A> * block_arg, const size_type & sort_row) {
        using iter = typename block<V,A>::iter;
        size_type dim0_ = block_arg->dim0();
        size_type rows_ = block_arg->num_rows();
        std::vector<iter> actual_starts;
        std::vector<V> ideal_col_order;
        // populate these
        for (size_type i = 0;i < dim0_;i++) {
            actual_starts.push_back(iter(block_arg,i,sort_row,0));
            ideal_col_order.push_back((*block_arg)(i,sort_row));
        }
        //sort these starts and ends according to the row value of interest
        std::sort(ideal_col_order.begin(),ideal_col_order.end(),[&actual_starts] (const V& a,const V& b)->bool {
                return a < b ? true:false ;});
        //use the stl swap_ranges in tandem with the 
        for (size_type col_ind = 0;col_ind < dim0_ ;col_ind++) {
            //check whethere the column is in the correct place already
            if (actual_starts[col_ind].get_value() == ideal_col_order[col_ind]) continue;
            //get correct iterators and use as arguments to swap_ranges
            iter && end {iter(block_arg,actual_starts[col_ind].get_col(),sort_row,rows_)};
            iter * swap_col = &actual_starts[col_ind];
            while (swap_col->get_value() != ideal_col_order[col_ind]) swap_col++;
            std::swap_ranges(actual_starts[col_ind],end,*swap_col);
            //update the actual vectors to reflectt changed block_arg state
            actual_starts[col_ind].set_col(swap_col->get_col());
            swap_col->set_col(col_ind);
            std::swap(actual_starts[col_ind],*swap_col);
            // continue
        }
    }
}
#endif
