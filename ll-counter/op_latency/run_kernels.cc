#include <iostream>
#include <vector>
#include <array>
#include <cstring>

#include "llc/ll_harness.h"
#include "llc/ll_counter_generic_perf.h"
#include "llc/tvalue_list.h"
#include "llc/pin_thread.h"

#include "latency_kernels.h"
#include "cxx_demangle.h"

#ifdef MINIMAL_HARNESS
#define HARNESS_ARGS 1,0,false
#else
#define HARNESS_ARGS 50000
#endif

#ifndef KSIZE
#define KSIZE 5
#endif

/** Scales kernel up to size max_ksize.
 *
 * Semantics depend on particular kernel.
 */

constexpr int max_ksize=KSIZE;

using namespace llc;

std::ostream &emit_header(std::ostream &O) {
    O << "type,\top,\tksize,\tcount,\tcount_harness\n";
    return O;
}

struct count_item {
    unsigned ksize;
    uint64_t c_kernel,c_harness;
};

template <typename ci_iter>
std::ostream &emit_counts(std::ostream &O,const std::string &arg_type,arith_op::arith_op op,ci_iter b,ci_iter e) {
    for (;b!=e;++b)
        O << arg_type << ",\t" << op << ",\t"
          << b->ksize << ",\t" << b->c_kernel << ",\t" << b->c_harness << "\n";

    return O;
}

template <typename FP,arith_op::arith_op OP,typename ksizes>
struct runner_looped_karg {
    template <unsigned n>
    struct eval {
        template <typename harness,typename ci_outiter,typename... A>
        static void run(harness &H,ci_outiter &ci,size_t n_inner,A... op_args) {
            H.run(kernel_looped_karg<FP,OP,n,max_ksize>::run,n_inner,op_args...);
            *ci++={n,H.min_count(),H.min_count_harness()};
        }
    };

    template <typename harness,typename... A>
    static std::ostream &run_and_emit(std::ostream &O,harness &H,size_t n_inner,A... op_args) {
        std::array<count_item,ksizes::length> count_data;

        count_item *ci=&count_data[0];
        ksizes::template for_each<eval>::run(H,ci,n_inner,op_args...);

        return emit_counts(O,type_name<FP>(),OP,count_data.begin(),count_data.end());
    }
};

// TODO: investigate value dependency on latency of arithmetical ops.

template <typename harness>
void run_all_kernels(harness &H) {
    emit_header(std::cout);
    pin_thread();

    size_t n_inner=100;

    typedef tvalue_seq<unsigned,0,max_ksize>::type ksizes;

    // up to three arguments for arithmetic ops.
    float f1=1.f,f2=2.f,f3=0.f;
    runner_looped_karg<float,arith_op::add,ksizes>::run_and_emit(std::cout,H,n_inner,f1,f2,f3);
    runner_looped_karg<float,arith_op::mul,ksizes>::run_and_emit(std::cout,H,n_inner,f1,f2,f3);
    runner_looped_karg<float,arith_op::div,ksizes>::run_and_emit(std::cout,H,n_inner,f1,f2,f3);
    runner_looped_karg<float,arith_op::sqrt,ksizes>::run_and_emit(std::cout,H,n_inner,f1,f2,f3);

    double d1=1.0,d2=2.0,d3=0.0;
    runner_looped_karg<double,arith_op::add,ksizes>::run_and_emit(std::cout,H,n_inner,d1,d2,d3);
    runner_looped_karg<double,arith_op::mul,ksizes>::run_and_emit(std::cout,H,n_inner,d1,d2,d3);
    runner_looped_karg<double,arith_op::div,ksizes>::run_and_emit(std::cout,H,n_inner,d1,d2,d3);
    runner_looped_karg<double,arith_op::sqrt,ksizes>::run_and_emit(std::cout,H,n_inner,d1,d2,d3);
}


int main(int argc,char **argv) {
    const char *counter=argv[1]?argv[1]:"perf_cycle";

    if (!std::strcmp(counter,"perf_cycle")) {
        ll_harness<ll_counter<generic_perf>,HARNESS_ARGS> H(perf_event_hw(PERF_COUNT_HW_CPU_CYCLES));
        run_all_kernels(H);
    }
    else if (!std::strcmp(counter,"perf_op")) {
        ll_harness<ll_counter<generic_perf>,HARNESS_ARGS> H(perf_event_hw(PERF_COUNT_HW_INSTRUCTIONS));
        run_all_kernels(H);
    }
    else {
        std::cerr << "usage: run_kernels [counter]\n"
                     "where counter is one of: perf_cycle, perf_op.\n";
        return 2;
    }
}
