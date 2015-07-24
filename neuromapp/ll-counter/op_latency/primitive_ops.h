#ifndef PRIMITIVE_OPS_H
#define PRIMITIVE_OPS_H

#include <cstddef>
#include <stdexcept>
#include <cmath>

#include "llc/ll_common.h"

namespace arith_op {
    enum arith_op { add, sub, mul, fma, div, sqrt, exp};
}

/** Typedefs for 128-bit SIMD vectors */

typedef float v4float __attribute((vector_size(16)));
typedef double v2double __attribute((vector_size(16)));

template <typename FP>
struct v_or_s_zero {
    constexpr static FP value=0;
};

template <>
struct v_or_s_zero<v4float> {
    constexpr static v4float value={0.f,0.f,0.f,0.f};
};

template <>
struct v_or_s_zero<v2double> {
    constexpr static v2double value={0.,0.};
};

/** Generic value sink
 *
 * Architecture-specific implementations will use
 * asm construction to provide data dependency without
 * explicit store as required.
 */

template <typename V>
inline void consume(V v) { volatile V u(v); }

/** Wrap underlying assembly for primitive arithmetic operation.
 *
 * Argument convention is:
 *     primitive_op<OP>::run(V &a1,V a2,...)
 * implements the arithmetic operation on values of type V
 *     a1 = OP(a1,a2,...)
 * where extra arguments are ignored.
 *
 * primitive_op<OP>, unless specialized by an architecture-specific
 * implementation, inherits from a basic C++ implementation of
 * the operation in primitive_op_default<OP>.
 *
 * Specializations should set primitive_op<OP>::is_specialized
 * to true.
 */

template <arith_op::arith_op op>
struct primitive_op_default {
    static void run(...) { throw std::invalid_argument("unsupported operation"); }
    static constexpr bool is_specialized=false; 
};

template <>
struct primitive_op_default<arith_op::add> {
    template <typename V>
    ALWAYS_INLINE static void run(V &a1,V a2,...) { a1+=a2; }
    static constexpr bool is_specialized=false; 
};

template <>
struct primitive_op_default<arith_op::sub> {
    template <typename V>
    ALWAYS_INLINE static void run(V &a1,V a2,...) { a1-=a2; }
    static constexpr bool is_specialized=false; 
};

template <>
struct primitive_op_default<arith_op::mul> {
    template <typename V>
    ALWAYS_INLINE static void run(V &a1,V a2,...) { a1*=a2; }
    static constexpr bool is_specialized=false; 
};

template <>
struct primitive_op_default<arith_op::fma> {
    template <typename V>
    ALWAYS_INLINE static void run(V &a1,V a2,V a3,...) { a1=std::fma(a1,a2,a3); }
    static constexpr bool is_specialized=false; 
};

template <>
struct primitive_op_default<arith_op::div> {
    template <typename V>
    ALWAYS_INLINE static void run(V &a1,V a2,...) { a1/=a2; }
    static constexpr bool is_specialized=false; 
};

template <>
struct primitive_op_default<arith_op::sqrt> {
    template <typename V>
    ALWAYS_INLINE static void run(V &a1,...) { a1=std::sqrt(a1); }
    ALWAYS_INLINE static void run(v4float &a1,...) {
        a1[0]=std::sqrt(a1[0]);
        a1[1]=std::sqrt(a1[1]);
        a1[2]=std::sqrt(a1[2]);
        a1[3]=std::sqrt(a1[3]);
    }
    ALWAYS_INLINE static void run(v2double &a1,...) {
        a1[0]=std::sqrt(a1[0]);
        a1[1]=std::sqrt(a1[1]);
    }
    static constexpr bool is_specialized=false; 
};

template <>
struct primitive_op_default<arith_op::exp> {
    template <typename V>
    ALWAYS_INLINE static void run(V &a1,...) { a1=std::exp(a1); }
    ALWAYS_INLINE static void run(v4float &a1,...) {
        a1[0]=std::exp(a1[0]);
        a1[1]=std::exp(a1[1]);
        a1[2]=std::exp(a1[2]);
        a1[3]=std::exp(a1[3]);
    }
    ALWAYS_INLINE static void run(v2double &a1,...) {
        a1[0]=std::exp(a1[0]);
        a1[1]=std::exp(a1[1]);
    }
    static constexpr bool is_specialized=false; 
};

template <arith_op::arith_op op>
struct primitive_op: primitive_op_default<op> {
};

/** arith_op to string conversion */

std::string to_string(arith_op::arith_op);
std::ostream &operator<<(std::ostream &,arith_op::arith_op);

// architecture-specific specializations:

#if defined(__powerpc) || defined(_M_PPC) || defined(_ARCH_PPC)
// actually, this needs to be specific to 2.07 ISA / POWER8
#include "primitive_ops_power8.h"
#elif defined(__x86_64) || defined(_M_X64)
#include "primitive_ops_x86_64.h"
#else
#warning "unsupported archictecture"
#endif

#endif // ndef PRIMITIVE_OPS_H
