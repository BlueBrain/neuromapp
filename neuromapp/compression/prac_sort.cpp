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
// todo change to variable type, and allocation method
typedef float value_type;
typedef size_t size_type;
using prac_block = block<value_type,cstandard>;

int main (void) {
    prac_block b1;
    //read data into block from file
    //string fname= "trans_data/values_9_a8788trans_bulk.csv";
    //string fname = "trans_data/8values.csv";
    //string fname = "trans_data/16values.csv";
    //string fname = "trans_data/4values.csv";
    string fname = "trans_data/even_easier.csv";
    //string fname= "data/csv/values_9_a10245bulk.csv";
    ifstream ifile(fname);
    ifile >> b1;
    std::cout << b1 << std::endl;
    //std::cout << "block is " << b1  << std::endl;
    //perform sort
    size_type row_sort = 0;
    neuromapp::col_sort(&b1,row_sort);
    std::cout << "PRINTING RESULT OF THE SORTING" << std::endl;
    std::cout << b1 << std::endl;
}
