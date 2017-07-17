#include <stdexcept>
#include <vector>
#include <fstream>
#include <algorithm>
#include "zlib.h"
#include <iostream>
#include <string>
#include "compression.h"
#include "block.h"
#include "compressor.h"
#include "block_sort.h"

#define BLOCK_SIZE 8000
//total bytes (type double) 64000000
using namespace std;
using neuromapp::block;
using neuromapp::cstandard;
typedef size_t size_type;



int main (void) {
    std::cout << "starting" << std::endl;
    block<double,cstandard> b1(BLOCK_SIZE);
    std::cout << "created block" << std::endl;
    //read data into block from file
    std::cout << "opening file" << std::endl;
    string fname = "trans_data/even_easier.csv";
    ifstream ifile(fname);
    std::cout << "about to read into block" << std::endl;
    ifile >> b1;
    std::cout << "finished read, now printing contents" << std::endl;
    std::cout << b1 << std::endl;
    //perform sort
    std::cout << "performing the sort" << std::endl;
    size_type row_sort = 0;
    neuromapp::col_sort(&b1,row_sort);
    b1.compress();
    b1.uncompress();
    std::cout << "PRINTING RESULT OF THE SORTING" << std::endl;
    std::cout << b1 << std::endl;
}
