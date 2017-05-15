// the includes are relative to the directory above
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block.h"
#include <iostream>

//include the boost program options later on
//first make sure that the compression option can be shown.
int comp_execute(int argc,char *const argv[])
{
    size_t n,m;
    n = 2;
    m = 30;
    using neuromapp::block;using neuromapp::cstandard;
    block<int,cstandard> my_block(n,m);
    std::cin >> my_block[1];
    for (auto subblock: my_block) {
        std::cout << subblock << std::endl;
    }
    return 0;
}

