#include <iostream>

#include "ll_counter.h"
#include "ll_counter_power8.h"

using namespace llc;

template <typename C>
__attribute((noinline)) typename C::value_type instrument_asm_add(C &counter,int a,int b) {
    int r=0,s=0;
#if 0
    counter.start();
    asm volatile("add %[r],%[a],%[b]\n\t":[r]"=r"(r):[a]"r"(a),[b]"b"(b): "memory");
    counter.stop();
#endif
    counter.reset();
    counter.instrument([&]() { 
        asm volatile("add %[r],%[a],%[b]\n\t"
                     "add %[s],%[r],%[r]\n\t"
                     "add %[r],%[s],%[s]\n\t"
                    :[r]"=&r"(r),[s]"=r"(s):[a]"r"(a),[b]"b"(b): "memory");
    });
    return counter.count();
}

int main() {
    constexpr int ntrial=10;
    uint64_t ops[ntrial],cyc[ntrial];

    volatile int arg_in[]={1,3};

    for (int i=0;i<ntrial;++i) {
        ll_counter<ppc_op> C_op;
        ll_counter<ppc_cycle> C_cycle;
        ops[i]=instrument_asm_add<ll_counter<ppc_op>>(C_op,arg_in[0],arg_in[1]);
        cyc[i]=instrument_asm_add<ll_counter<ppc_cycle>>(C_cycle,arg_in[0],arg_in[1]);
    }
    for (int i=0;i<ntrial;++i) 
        std::cout << "c_add: " << ops[i] << "/" << cyc[i] << "\n";
}
