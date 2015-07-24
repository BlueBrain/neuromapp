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

template <> std::string type_name<v4float>() { return "v4f"; }
template <> std::string type_name<v2double>() { return "v2d"; }

std::ostream &emit_header(std::ostream &O) {
    O << "tag,\ttype,\top,\tspec,\tksize,\tcount,\tcount_harness\n";
    return O;
}

struct count_item {
    unsigned ksize;
    uint64_t c_kernel,c_harness;
};

template <typename ci_iter>
std::ostream &emit_counts(std::ostream &O,const std::string &tag,const std::string &arg_type,op_enum op,bool spec,ci_iter b,ci_iter e) {
    for (;b!=e;++b)
        O << tag << ",\t" << arg_type << ",\t" << op << ",\t" << std::boolalpha << spec << ",\t"
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
    static std::ostream &run(std::ostream &O,const std::string &tag,harness &H,A&&... kernel_args) {
        std::array<count_item,ksizes::length> count_data;

        count_item *ci=&count_data[0];
        ksizes::template for_each<eval>::run(H,ci,std::forward<A>(kernel_args)...);

        return emit_counts(O,tag,type_name<FP>(),OP,primitive_op<OP>::is_specialized,count_data.begin(),count_data.end());
    }
};

// TODO: investigate value dependency on latency of arithmetical ops.

typedef tvalue_seq<unsigned,0,max_ksize>::type ksizes;

void run_setup() {
    emit_header(std::cout);
    pin_thread();
}

void set_default_operands(float &f1,float &f2,float &f3,double &d1,double &d2,double &d3) {
    d1=1.1e-7;
    d2=2.1;
    d3=3.2;

    f1=(float)d1;
    f2=(float)d2;
    f3=(float)d3;
}

template <op_enum OP> using run_float_dep_seq = run_kernels<kernel_dep_seq,float,OP,ksizes>;
template <op_enum OP> using run_double_dep_seq = run_kernels<kernel_dep_seq,double,OP,ksizes>;

template <op_enum OP> using run_v4float_dep_seq = run_kernels<kernel_dep_seq,v4float,OP,ksizes>;
template <op_enum OP> using run_v2double_dep_seq = run_kernels<kernel_dep_seq,v2double,OP,ksizes>;

template <typename harness>
void run_dep_seq_kernels(harness &H) {
    run_setup();

    float f1,f2,f3;
    double d1,d2,d3;
    set_default_operands(f1,f2,f3,d1,d2,d3);

    // do not include exp for non-constant argument latency kernel
    typedef tvalue_list<op_enum,arith_op::add,arith_op::mul,arith_op::fma,arith_op::div,arith_op::sqrt> ops;

    ops::template for_each<run_float_dep_seq>::run(std::cout,"default",H,f1,f2,f3);
    ops::template for_each<run_double_dep_seq>::run(std::cout,"default",H,d1,d2,d3);

    typedef ops vops;

    v4float vf1={f1,f1,f1,f1},vf2={f2,f2,f2,f2},vf3={f3,f3,f3,f3};
    v2double vd1={d1,d1},vd2={d2,d2},vd3={d3,d3};

    vops::template for_each<run_v4float_dep_seq>::run(std::cout,"default",H,vf1,vf2,vf3);
    vops::template for_each<run_v2double_dep_seq>::run(std::cout,"default",H,vd1,vd2,vd3);
}

template <op_enum OP> using run_float_looped_seq = run_kernels<kernel_looped_seq,float,OP,ksizes>;
template <op_enum OP> using run_double_looped_seq = run_kernels<kernel_looped_seq,double,OP,ksizes>;

template <op_enum OP> using run_v4float_looped_seq = run_kernels<kernel_looped_seq,v4float,OP,ksizes>;
template <op_enum OP> using run_v2double_looped_seq = run_kernels<kernel_looped_seq,v2double,OP,ksizes>;

template <typename harness>
void run_looped_seq_kernels(harness &H) {
    run_setup();

    float f1,f2,f3;
    double d1,d2,d3;
    set_default_operands(f1,f2,f3,d1,d2,d3);

    size_t n_inner=100;

    // do not include exp for non-constant argument latency kernel
    typedef tvalue_list<op_enum,arith_op::add,arith_op::mul,arith_op::fma,arith_op::div,arith_op::sqrt> ops;

    ops::template for_each<run_float_looped_seq>::run(std::cout,"default",H,n_inner,f1,f2,f3);
    ops::template for_each<run_double_looped_seq>::run(std::cout,"default",H,n_inner,d1,d2,d3);

    typedef ops vops;

    v4float vf1={f1,f1,f1,f1},vf2={f2,f2,f2,f2},vf3={f3,f3,f3,f3};
    v2double vd1={d1,d1},vd2={d2,d2},vd3={d3,d3};

    vops::template for_each<run_v4float_looped_seq>::run(std::cout,"default",H,n_inner,vf1,vf2,vf3);
    vops::template for_each<run_v2double_looped_seq>::run(std::cout,"default",H,n_inner,vd1,vd2,vd3);
}

template <op_enum OP> using run_float_looped_karg = run_kernels<kernel_looped_karg,float,OP,ksizes>;
template <op_enum OP> using run_double_looped_karg = run_kernels<kernel_looped_karg,double,OP,ksizes>;

template <op_enum OP> using run_v4float_looped_karg = run_kernels<kernel_looped_karg,v4float,OP,ksizes>;
template <op_enum OP> using run_v2double_looped_karg = run_kernels<kernel_looped_karg,v2double,OP,ksizes>;

template <typename harness>
void run_looped_karg_kernels(harness &H) {
    run_setup();

    float f1,f2,f3;
    double d1,d2,d3;
    set_default_operands(f1,f2,f3,d1,d2,d3);

    size_t n_inner=100;

    typedef tvalue_list<op_enum,arith_op::add,arith_op::mul,arith_op::fma,arith_op::div,arith_op::sqrt,arith_op::exp> ops;

    ops::template for_each<run_float_looped_karg>::run(std::cout,"default",H,n_inner,f1,f2,f3);
    ops::template for_each<run_double_looped_karg>::run(std::cout,"default",H,n_inner,d1,d2,d3);

    typedef ops vops;

    v4float vf1={f1,f1,f1,f1},vf2={f2,f2,f2,f2},vf3={f3,f3,f3,f3};
    v2double vd1={d1,d1},vd2={d2,d2},vd3={d3,d3};

    vops::template for_each<run_v4float_looped_karg>::run(std::cout,"default",H,n_inner,vf1,vf2,vf3);
    vops::template for_each<run_v2double_looped_karg>::run(std::cout,"default",H,n_inner,vd1,vd2,vd3);
}

template <typename harness>
void run_argdep_karg_kernels(harness &H) {
    run_setup();

    float f1,f2,f3;
    double d1,d2,d3;
    set_default_operands(f1,f2,f3,d1,d2,d3);

    size_t n_inner=100;

    /* Division and sqrt latency tests
     *
     * op(A,B) and op(A) where A and B are one of the table below.
     * P and Q are chosen so that we may get float overflow or underflow.
     * X is chosen so that X/D should not overflow.
     */

    typedef tvalue_list<op_enum,arith_op::add,arith_op::mul,arith_op::div> binary_ops;
    typedef tvalue_list<op_enum,arith_op::sqrt> unary_ops;

    struct { float f; char tag; } foperands[] = {
        { 0x0p0f,          'Z'},     // 0.0             zero
        { 0x1p0f,          'U'},     // 1.0             unit
        { 0x1p80f,         'P'},     // 2^80            power of two
        { -0x1p-81f,       'Q'},     // -2^-81          negative power of two
        { 0x1.123456p-30f, 'X'},     // 9.97549932e-10  unremarkable number
        { 0x0.fedcbap-127f,'D'},     // 1.17026989e-38  denormal number
        { 0, 0}
    };

    for (auto f1p=&foperands[0];f1p->tag;++f1p) {
        for (auto f2p=&foperands[0];f2p->tag;++f2p) {
            char tag[3]={f1p->tag,f2p->tag,0};
            binary_ops::template for_each<run_float_looped_karg>::run(std::cout,tag,H,n_inner,f1p->f,f2p->f,0.f);
        }
        char tag[2]={f1p->tag,0};
        unary_ops::template for_each<run_float_looped_karg>::run(std::cout,tag,H,n_inner,f1p->f,0.f,0.f);
    }

    struct { double d; char tag; } doperands[] = {
        { 0x0p0,           'Z'},          // 0.0              zero
        { 0x1p0,           'U'},          // 1.0              unit
        { 0x1p600,         'P'},          // 2^600            power of two
        { -0x1p-601,       'Q'},          // -2^-601          negative power of two
        { 0x1.123456789abcdp-30,   'X'},  // 9.2904079491202317e-19  unremarkable number
        { 0x0.fedcba9876543p-1022, 'D'},  // 2.2151846413582804e-308 denormal number
        { 0, 0}
    };

    for (auto d1p=&doperands[0];d1p->tag;++d1p) {
        for (auto d2p=&doperands[0];d2p->tag;++d2p) {
            char tag[3]={d1p->tag,d2p->tag,0};
            binary_ops::template for_each<run_double_looped_karg>::run(std::cout,tag,H,n_inner,d1p->d,d2p->d,0.);
        }
        char tag[2]={d1p->tag,0};
        unary_ops::template for_each<run_double_looped_karg>::run(std::cout,tag,H,n_inner,d1p->d,0.f,0.f);
    }
}

int main(int argc,char **argv) {
    /* dispatch on combination of counter and kernel */
    enum which_counter { perf_cycle, perf_op } counter=perf_cycle;
    enum which_kernel { looped_karg, looped_seq, dep_seq, argdep_karg } kernel=looped_karg;

    for (int i=1;i<argc;++i) {
        auto arg_is=[&](const char *s) -> bool { return !std::strcmp(s,argv[i]); };

        if (arg_is("perf_cycle")) counter=perf_cycle;
        else if (arg_is("perf_op")) counter=perf_op;
        else if (arg_is("looped_karg")) kernel=looped_karg;
        else if (arg_is("argdep_karg")) kernel=argdep_karg;
        else if (arg_is("looped_seq")) kernel=looped_seq;
        else if (arg_is("dep_seq")) kernel=dep_seq;
        else {
            std::cerr << "usage: op_latency [COUNTER] [KERNEL]\n"
                         "where COUNTER is one of: perf_cycle, perf_op\n"
                         "      KERNEL is one of:  looped_karg, looped_seq, dep_seq, argdep_karg\n";
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
    case argdep_karg: run_argdep_karg_kernels(H); break;
    case looped_seq:  run_looped_seq_kernels(H); break;
    case dep_seq:     run_dep_seq_kernels(H); break;
    default:          return 1; // should not happen
    }

    return 0;
}
