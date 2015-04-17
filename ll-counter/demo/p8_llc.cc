#include <iostream>
#include <cassert>

#if defined(__powerpc) || defined(_M_PPC) || defined(_ARCH_PPC)

#include "llc/ll_counter.h"
#include "llc/ll_counter_power8.h"
#include "llc/pin_thread.h"

using namespace llc;

template <typename C>
float instrument_asm_add_loop(C &counter,size_t n,float a,float b) {
    float r;
    counter.reset();
    counter.instrument([n,a,b,&r]() { 
        // set r to a and add b to r n times.
        asm volatile("fmr %[r],%[a]\n\t"
                     "mtctr %[n]\n\t"
                     ".p2align 4\n\t"
                     "1:\n\t"
                     "fadds %[r],%[r],%[b]\n\t"
                     "bdnz 1b\n\t"
                     : [r]"=&f"(r)
                     : [n]"r"(n),[a]"f"(a),[b]"f"(b)
                     : "ctr");

    });
    return r;
}

int main() {
    constexpr int ntrial=10;
    uint64_t ops[ntrial],cyc[ntrial];

    pin_thread();
    for (int i=0;i<ntrial;++i) {
        ll_counter<ppc_op> C_op;
        ll_counter<ppc_cycle> C_cycle;
        size_t n=1000;

        float r;
        r=instrument_asm_add_loop(C_op,n,1.f,3.f);
        assert(r==1.f+n*3.f);
        r=instrument_asm_add_loop(C_cycle,n,1.f,3.f);
        assert(r==1.f+n*3.f);

        ops[i]=C_op.count();
        cyc[i]=C_cycle.count();
    }

    for (int i=0;i<ntrial;++i) 
        std::cout << "trial " << (i+1) << ":\top: " << ops[i] << "\tcyc: " << cyc[i] << "\n";
}

#else

int main() {
    std::cerr << "incorrect architecture\n";
    return 1;
}

#endif
