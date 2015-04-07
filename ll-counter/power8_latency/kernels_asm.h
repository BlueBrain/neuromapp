#ifndef KERNELS_ASM_H
#define KERNELS_ASM_H

#include "unroll.h"
#include "ll_common.h"
#include "ll_counter_power8.h"

using namespace llc;

// serial fp arithmetic sequence with dependencies

namespace arith_op {
    enum arith_op { add, mul, div, sqrt };
}

template <typename V,arith_op::arith_op op,unsigned k>
struct serial_arithmetic;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"

template <unsigned k>
struct serial_arithmetic<double,arith_op::add,k> {
    ALWAYS_INLINE static void run() {
        ll_compiler_fence();
        double x;
        unroll<k>::run([&]() {
                    asm volatile ("fadd %0,%0,%0\n\t":"=d"(x));
                });
        ll_compiler_fence();
    }
};

template <unsigned k>
struct serial_arithmetic<double,arith_op::mul,k> {
    ALWAYS_INLINE static void run() {
        ll_compiler_fence();
        double x;
        unroll<k>::run([&]() {
                    asm volatile ("fmul %0,%0,%0\n\t":"=d"(x));
                });
        ll_compiler_fence();
    }
};

template <unsigned k>
struct serial_arithmetic<double,arith_op::div,k> {
    ALWAYS_INLINE static void run() {
        ll_compiler_fence();
        double x;
        unroll<k>::run([&]() {
                    asm volatile ("fdiv %0,%0,%0\n\t":"=d"(x));
                });
        ll_compiler_fence();
    }
};

template <unsigned k>
struct serial_arithmetic<double,arith_op::sqrt,k> {
    ALWAYS_INLINE static void run() {
        ll_compiler_fence();
        double x;
        unroll<k>::run([&]() {
                    asm volatile ("xssqrtdp %0,%0\n\t":"=wa"(x));
                });
        ll_compiler_fence();
    }
};

template <unsigned k>
struct serial_arithmetic<float,arith_op::add,k> {
    ALWAYS_INLINE static void run() {
        ll_compiler_fence();
        float x;
        unroll<k>::run([&]() {
                    asm volatile ("fadds %0,%0,%0\n\t":"=f"(x));
                });
        ll_compiler_fence();
    }
};

template <unsigned k>
struct serial_arithmetic<float,arith_op::mul,k> {
    ALWAYS_INLINE static void run() {
        ll_compiler_fence();
        float x;
        unroll<k>::run([&]() {
                    asm volatile ("fmuls %0,%0,%0\n\t":"=f"(x));
                });
        ll_compiler_fence();
    }
};

template <unsigned k>
struct serial_arithmetic<float,arith_op::div,k> {
    ALWAYS_INLINE static void run() {
        ll_compiler_fence();
        float x;
        unroll<k>::run([&]() {
                    asm volatile ("fdivs %0,%0,%0\n\t":"=f"(x));
                });
        ll_compiler_fence();
    }
};

template <unsigned k>
struct serial_arithmetic<float,arith_op::sqrt,k> {
    ALWAYS_INLINE static void run() {
        ll_compiler_fence();
        float x;
        unroll<k>::run([&]() {
                    asm volatile ("xssqrtsp %0,%0\n\t":"=wa"(x));
                });
        ll_compiler_fence();
    }
};

#pragma GCC diagnostic pop


#endif // ndef KERNELS_ASM_H
