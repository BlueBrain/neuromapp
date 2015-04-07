#include <iostream>
#include <vector>
#include <array>
#include <cstring>

#include "ll_harness.h"
#include "kernels_asm.h"
#include "ll_counter_power8.h"
#include "ll_counter_generic_perf.h"
#include "pin_thread.h"

#ifdef MINIMAL_HARNESS
#define HARNESS_ARGS 1,0,false
#else
#define HARNESS_ARGS 10000
#endif

const char *op_name(arith_op::arith_op op) {
    switch (op) {
    case arith_op::add: return "add";
    case arith_op::mul: return "mul";
    case arith_op::div: return "div";
    case arith_op::sqrt: return "sqrt";
    default: return "???";
    }
}

template <typename FP>
const char *type_name();

template<> const char *type_name<double>() { return "double"; }
template<> const char *type_name<float>() { return "float"; }

std::ostream &emit_header(std::ostream &O) {
    O << "type,\top,\tksize,\tcount,\tcount_harness\n";
    return O;
}


template <typename count_container>
std::ostream &emit_counts(std::ostream &O,const char *arg_type,arith_op::arith_op op,int kbase,const count_container &c_kernel,const count_container &c_harness) {
    auto ci=c_kernel.begin();
    auto ci_end=c_kernel.end();

    auto ch=c_harness.begin();
    auto ch_end=c_harness.end();

    int k=kbase;

    while (ci!=ci_end && ch!=ch_end) 
        O << arg_type << ",\t" << op_name(op) << ",\t"
          << k++ << ",\t" << *ci++ << ",\t" << *ch++ << "\n";

    return O;
}

template <typename FP,arith_op::arith_op OP>
struct serial_arithmetic_runner {
    template <int n>
    struct eval {
        template <typename harness,typename op_cyc_outiter,typename h_cyc_outiter>
        static void run(harness &H,op_cyc_outiter &op_iter,h_cyc_outiter &h_iter) {
            H.run<serial_arithmetic<float,arith_op::mul,n>::run>();
            *op_iter++=H.min_count();
            *h_iter++=H.min_count_harness();
        }
    };

    template <int upto,typename harness>
    static std::ostream &run_and_emit(std::ostream &O,harness &H) {
        std::array<uint64_t,upto> op_cyc,h_cyc;

        auto op_iter=&op_cyc[0];
        auto h_iter=&h_cyc[0];
        unroll_static_n<upto,eval,1>::run(H,op_iter,h_iter);

        return emit_counts(O,type_name<FP>(),OP,1,op_cyc,h_cyc);
    }
};

template <unsigned upto,typename harness>
void run_all_kernels(harness &H) {
    emit_header(std::cout);
    pin_thread();

    serial_arithmetic_runner<float,arith_op::add>::run_and_emit<upto>(std::cout,H);
    serial_arithmetic_runner<float,arith_op::mul>::run_and_emit<upto>(std::cout,H);
    serial_arithmetic_runner<float,arith_op::div>::run_and_emit<upto>(std::cout,H);
    serial_arithmetic_runner<float,arith_op::sqrt>::run_and_emit<upto>(std::cout,H);

    serial_arithmetic_runner<double,arith_op::add>::run_and_emit<upto>(std::cout,H);
    serial_arithmetic_runner<double,arith_op::mul>::run_and_emit<upto>(std::cout,H);
    serial_arithmetic_runner<double,arith_op::div>::run_and_emit<upto>(std::cout,H);
    serial_arithmetic_runner<double,arith_op::sqrt>::run_and_emit<upto>(std::cout,H);
}

int main(int argc,char **argv) {
#ifndef KSIZE
#define KSIZE 10
#endif
    constexpr int max_ksize=KSIZE;

    const char *counter=argv[1]?argv[1]:"perf_cycle";

    if (!std::strcmp(counter,"perf_cycle")) {
        ll_harness<ll_counter<generic_perf>,HARNESS_ARGS> H(perf_event_hw(PERF_COUNT_HW_CPU_CYCLES));
        run_all_kernels<max_ksize>(H);
    }
    else if (!std::strcmp(counter,"perf_op")) {
        ll_harness<ll_counter<generic_perf>,HARNESS_ARGS> H(perf_event_hw(PERF_COUNT_HW_INSTRUCTIONS));
        run_all_kernels<max_ksize>(H);
    }
    else if (!std::strcmp(counter,"ppc_cycle")) {
        ll_harness<ll_counter<ppc_cycle>,HARNESS_ARGS> H;
        run_all_kernels<max_ksize>(H);
    }
    else if (!std::strcmp(counter,"ppc_op")) {
        ll_harness<ll_counter<ppc_op>,HARNESS_ARGS> H;
        run_all_kernels<max_ksize>(H);
    }
    else {
        std::cerr << "usage: run_kernels [counter]\n"
                     "where counter is one of: perf_cycle, perf_op, ppc_cycle, ppc_op.\n";
        return 2;
    }
}
