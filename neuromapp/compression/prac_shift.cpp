#include <iostream>
#include <fstream>
#include "compression.h"
#include "block.h"
#include "bit_shifting.h"
using namespace std;


using neuromapp::block;
using neuromapp::cstandard;
// todo change to variable type, and allocation method
typedef float value_type;
typedef float * pointer;
using prac_block = block<value_type,cstandard>;
int main (void) {
    prac_block b1;
    string fname = "trans_data/even_easier.csv";
    ifstream ifile(fname);
    ifile >> b1;
    std::cout << b1 << std::endl;
    // not actually limited to the row otherwise
    size_type cols_ = b1.dim0(), rows_ = b1.num_rows();
    // copy has 3times as many columns and same number of rows
    block<unsigned int, cstandard> parts_block(cols_*3,rows_);
    neuromapp::populate_block_by_parts(parts_block,b1);
    std::cout << "parts becomes \n" << parts_block << std::endl;
}
