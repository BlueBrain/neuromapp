/* this file is here to give me a chance to practice compression in a way that isn't constrained by the program_options interface, and then later we can just include the functions drafted here*/
#include <sys/stat.h>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "zlib.h"
#include "compressor.h"
#include "allocator.h"
#include "exception.h"
#include "block.h"

#define BLOCK_SIZE 5000
#define ARRAY_SIZE 10
using neuromapp::block;
using neuromapp::cstandard;
// todo change to variable type, and allocation method
using prac_block = block<int,cstandard>;
typedef size_t size_type;

// testing the pragma_omp for with compression just in general
//
int main () {
    vector<prac_block> set_of_blocks(ARRAY_SIZE);
//#pragma omp parallel for
    for (int i = 0; i < ARRAY_SIZE;i++) {
        prac_block b((size_type) BLOCK_SIZE);
        b.compress();
        set_of_blocks[i] = b;
        std::cout << i <<"\n" << std::flush;
    }
}
