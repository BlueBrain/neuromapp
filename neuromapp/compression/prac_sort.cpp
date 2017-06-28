#include <stdexcept>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <string>
#include "compression.h"
#include "block.h"
#include "block_sort.h"
using namespace std;


using neuromapp::block;
using neuromapp::cstandard;
using neuromapp::Sorter;
// todo change to variable type, and allocation method
typedef float value_type;
using prac_block = block<value_type,cstandard>;

int main (void) {
    prac_block b1;
    //read data into block from file
    //string fname= "trans_data/values_9_a8788trans_bulk.csv";
    string fname= "data/csv/values_10_a8761bulk.csv";
    ifstream ifile(fname);
    ifile >> b1;
    std::cout << "successful read, cols are "<< b1.num_cols() << "and the () operator gives " <<
        b1(4,0)<< std::endl;
    //std::cout << "block is " << b1  << std::endl;
    //perform sort
    b1.col_sort(0);
    std::cout << "PRINTING RESULT OF THE SORTING" << std::endl;
    std::cout << b1 << std::endl;
}
