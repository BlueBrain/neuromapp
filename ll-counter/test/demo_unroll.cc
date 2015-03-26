/* Compile with -fstack-usage to get stack information
 * to confirm inlining state */

#include <iostream>
#include <cassert>

#if __GLIBC__==2
#define HAS_BACKTRACE
#include <execinfo.h>
#endif


#include "unroll.h"

using namespace llc;

// check iterations with unroll::run

void check_unroll_run() {
    int sum=0;
    for (int i=0;i<10;++i)
        unroll<4>::run([&](int j) { sum+=j; }, 3);

    assert(sum==10*4*3);
}

// check iterations and index with unroll::runi

void check_unroll_runi() {
    int sum=0;
    // sum 1..100
    for (int i=0;i<100;i+=4)
        unroll<4>::runi(i+1,[&](int i) { sum+=i; });

    assert(sum==101*100/2);
}

#ifdef HAS_BACKTRACE
int stack_depth() {
    constexpr size_t max_depth=100;
    void *backtrace_buf[max_depth];

    return backtrace(backtrace_buf,max_depth);
}

// check unroll isn't performing recursive calls

void check_unroll_stack_depth() {
    int depths[8];

    unroll<8>::runi(0,[&](int i){ depths[i]=stack_depth(); });
    for (int j=0;j<8;++j) {
        std::cout << "depth[" << j << "]==" << depths[j] << "\n";
    }
}

#else

void check_unroll_stack_depth() {
     std::cout << "not available\n";
}

#endif

int main() {
    check_unroll_stack_depth();
}



