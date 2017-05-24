/* this file is here to give me a chance to practice compression in a way that isn't constrained by the program_options interface, and then later we can just include the functions drafted here*/
#include <sys/stat.h>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "compression/compression.h"
#include "/usr/include/zlib.h"
#include "compression/zlib_fns.h"
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block.h"


using neuromapp::block;
using neuromapp::cstandard;
// todo change to variable type, and allocation method
using prac_block = block<int,cstandard>;

int main (void) {
    //do call rom the zlib_funs
    prac_block * demo_block = new prac_block(30,30);// figure out which is column and which is the row
    demo_block->enter_data("../memvolts.csv");// load in the data
    demo_block->print();
    zlib::block_compress(demo_block);
}
