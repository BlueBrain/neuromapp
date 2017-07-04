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
    pointer row = &b1(0,0);
    unsigned int * parts_of_row = neuromapp::convert_to_parts(row,b1.num_cols());
    for (size_type i = 0 ; i < b1.num_cols() ; i++) {
        size_type j;
        for (  j = 0; j < 3; j++ ) {
            std::cout << parts_of_row[i*3 + j]<< ", " << std::endl;
        }
    }
}
