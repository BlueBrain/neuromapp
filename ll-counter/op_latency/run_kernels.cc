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
typedef arith_op::arith_op op_enum;

std::ostream &emit_header(std::ostream &O) {
    O << "type,\top,\tspec,\tksize,\tcount,\tcount_harness\n";
    return O;
}

struct count_item {
    unsigned ksize;
    uint64_t c_kernel,c_harness;
};

template <typename ci_iter>
std::ostream &emit_counts(std::ostream &O,const std::string &arg_type,op_enum op,bool spec,ci_iter b,ci_iter e) {
    for (;b!=e;++b)
        O << arg_type << ",\t" << op << ",\t" << std::boolalpha << spec << ",\t"
          << b->ksize << ",\t" << b->c_kernel << ",\t" << b->c_harness << "\n";

    return O;
}

template <template <typename,op_enum,unsigned,unsigned> class kernel,typename FP,op_enum OP,typename ksizes>
struct run_kernels {
    template <unsigned n>
    struct eval {
        template <typename harness,typename ci_outiter,typename... A>
        static void run(harness &H,ci_outiter &ci,A&&... kernel_args) {
            H.run(kernel<FP,OP,n,max_ksize>::run,std::forward<A>(kernel_args)...);
            *ci++={n,H.min_count(),H.min_count_harness()};
        }
    };

    template <typename harness,typename... A>
    static std::ostream &run(std::ostream &O,harness &H,A&&... kernel_args) {
        std::array<count_item,ksizes::length> count_data;

        count_item *ci=&count_data[0];
        ksizes::template for_each<eval>::run(H,ci,std::forward<A>(kernel_args)...);

        return emit_counts(O,type_name<FP>(),OP,primitive_op<OP>::is_specialized,count_data.begin(),count_data.end());
    }
};

// TODO: investigate value dependency on latency of arithmetical ops.

typedef tvalue_seq<unsigned,0,max_ksize>::type ksizes;

void run_setup() {
    emit_header(std::cout);
    pin_thread();
}

template <op_enum OP> using run_float_dep_seq = run_kernels<kernel_dep_seq,float,OP,ksizes>;
template <op_enum OP> using run_double_dep_seq = run_kernels<kernel_dep_seq,double,OP,ksizes>;

template <typename harness>
void run_dep_seq_kernels(harness &H) {
    run_setup();

    typedef tvalue_list<op_enum,arith_op::add,arith_op::mul,arith_op::fma,arith_op::div,arith_op::sqrt,arith_op::exp> ops;

    float f1=1.f,f2=2.f,f3=0.f;
    ops::template for_each<run_float_dep_seq>::run(std::cout,H,f1,f2,f3);

    double d1=1.0,d2=2.0,d3=0.0;
    ops::template for_each<run_double_dep_seq>::run(std::cout,H,d1,d2,d3);
}

template <op_enum OP> using run_float_looped_karg = run_kernels<kernel_looped_karg,float,OP,ksizes>;
template <op_enum OP> using run_double_looped_karg = run_kernels<kernel_looped_karg,double,OP,ksizes>;

template <typename harness>
void run_looped_karg_kernels(harness &H) {
    run_setup();

    size_t n_inner=100;

    typedef tvalue_list<op_enum,arith_op::add,arith_op::mul,arith_op::fma,arith_op::div,arith_op::sqrt,arith_op::exp> ops;

    float f1=1.f,f2=2.f,f3=0.f;
    ops::template for_each<run_float_looped_karg>::run(std::cout,H,n_inner,f1,f2,f3);

    double d1=1.0,d2=2.0,d3=0.0;
    ops::template for_each<run_double_looped_karg>::run(std::cout,H,n_inner,d1,d2,d3);
}

template <op_enum OP> using run_float_looped_seq = run_kernels<kernel_looped_seq,float,OP,ksizes>;
template <op_enum OP> using run_double_looped_seq = run_kernels<kernel_looped_seq,double,OP,ksizes>;

template <typename harness>
void run_looped_seq_kernels(harness &H) {
    run_setup();

    size_t n_inner=100;

    typedef tvalue_list<op_enum,arith_op::add,arith_op::mul,arith_op::fma,arith_op::div,arith_op::sqrt,arith_op::exp> ops;

    float f1=1.f,f2=2.f,f3=0.f;
    ops::template for_each<run_float_looped_seq>::run(std::cout,H,n_inner,f1,f2,f3);

    double d1=1.0,d2=2.0,d3=0.0;
    ops::template for_each<run_double_looped_seq>::run(std::cout,H,n_inner,d1,d2,d3);
}

int main(int argc,char **argv) {
    /* dispatch on combination of counter and kernel */
    enum which_counter { perf_cycle, perf_op } counter=perf_cycle;
    enum which_kernel { looped_karg, looped_seq, dep_seq } kernel=looped_karg;

    for (int i=1;i<argc;++i) {
        auto arg_is=[&](const char *s) -> bool { return !std::strcmp(s,argv[i]); };

        if (arg_is("perf_cycle")) counter=perf_cycle;
        else if (arg_is("perf_op")) counter=perf_op;
        else if (arg_is("looped_karg")) kernel=looped_karg;
        else if (arg_is("looped_seq")) kernel=looped_seq;
        else if (arg_is("dep_seq")) kernel=dep_seq;
        else {
            std::cerr << "usage: op_latency [COUNTER] [KERNEL]\n"
                         "where COUNTER is one of: perf_cycle, perf_op\n"
                         "      KERNEL is one of:  looped_karg, looped_seq, dep_seq\n";
            return 2;
        } 
    }

    // dispatch accordingly ...
    perf_event ev;
    switch (counter) {
    case perf_cycle: ev=perf_event_hw(PERF_COUNT_HW_CPU_CYCLES); break;
    case perf_op:    ev=perf_event_hw(PERF_COUNT_HW_INSTRUCTIONS); break;
    default:         return 1; // should not happen
    }
    ll_harness<ll_counter<generic_perf>,HARNESS_ARGS> H(ev);

    switch (kernel) {
    case looped_karg: run_looped_karg_kernels(H); break;
    case looped_seq:  run_looped_seq_kernels(H); break;
    case dep_seq:     run_dep_seq_kernels(H); break;
    default:          return 1; // should not happen
    }

    return 0;
}
