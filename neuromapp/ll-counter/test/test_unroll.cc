#include <iostream>
#include <cassert>

#if __GLIBC__==2
#define HAS_BACKTRACE
#include <execinfo.h>
#endif

#include "llc/unroll.h"
#include "gtest/gtest.h"


using namespace llc;

// check iterations with unroll::run

TEST(unroll,run4) {
    int sum=0;
    for (int i=0;i<10;++i)
        unroll<4>::run([&](int j) { sum+=j; }, 3);

    ASSERT_EQ(sum,10*4*3);
}

// check iterations and index with unroll::runi

TEST(unroll,runi4) {
    int sum=0;
    // sum 1..100
    for (int i=0;i<100;i+=4)
        unroll<4>::runi(i+1,[&](int i) { sum+=i; });

    ASSERT_EQ(sum,101*100/2);
}

#ifdef HAS_BACKTRACE

int stack_depth() {
    constexpr size_t max_depth=100;
    void *backtrace_buf[max_depth];

    return backtrace(backtrace_buf,max_depth);
}

// check unroll isn't performing recursive calls

TEST(unroll,stack_depth) {
    constexpr unsigned n_unroll=8;
    int depth[n_unroll];

    for (int i=0;i<n_unroll;++i) depth[i]=-1;

    unroll<8>::runi(0,[&](int i){ depth[i]=stack_depth(); });
    
    EXPECT_GT(depth[0],-1);

    for (int j=1;j<n_unroll;++j) {
        EXPECT_EQ(depth[0],depth[j]);
    }
}

#endif

