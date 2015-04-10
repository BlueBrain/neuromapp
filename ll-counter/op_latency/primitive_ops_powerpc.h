#ifndef PRIMITIVE_OPS_POWERPC_H
#define PRIMITIVE_OPS_POWERPC_H

/** Implementation of primitive ops for POWER8 (and other PPC?).
 *
 *  Should be included automatically from primitive_ops.h and not directly by
 *  other code.
 */

template <>
struct primitive_op<arith_op::add> {
    ALWAYS_INLINE static void run(float &a1,float a2,...) {
        asm volatile ("fadds %0,%1,%2\n\t" :"=f"(a1) :"0"(a1),"f"(a2));
    }
    ALWAYS_INLINE static void run(double &a1,double a2,...) {
        asm volatile ("fadd %0,%1,%2\n\t" :"=d"(a1) :"0"(a1),"d"(a2));
    }
};

template <>
struct primitive_op<arith_op::mul> {
    ALWAYS_INLINE static void run(float &a1,float a2,...) {
        asm volatile ("fmuls %0,%1,%2\n\t" :"=f"(a1) :"0"(a1),"f"(a2));
    }
    ALWAYS_INLINE static void run(double &a1,double a2,...) {
        asm volatile ("fmul %0,%1,%2\n\t" :"=d"(a1) :"0"(a1),"d"(a2));
    }
};

template <>
struct primitive_op<arith_op::div> {
    ALWAYS_INLINE static void run(float &a1,float a2,...) {
        asm volatile ("fdivs %0,%1,%2\n\t" :"=f"(a1) :"0"(a1),"f"(a2));
    }
    ALWAYS_INLINE static void run(double &a1,double a2,...) {
        asm volatile ("fdiv %0,%1,%2\n\t" :"=d"(a1) :"0"(a1),"d"(a2));
    }
};


// TODO: include guard for VSX-specific operations ...

template <>
struct primitive_op<arith_op::sqrt> {
    ALWAYS_INLINE static void run(float &a1,...) {
        asm volatile ("xssqrtsp %0,%1\n\t" :"=wa"(a1) :"0"(a1));
    }
    ALWAYS_INLINE static void run(double &1,...) {
        asm volatile ("xssqrtdp %0,%1\n\t" :"=wa"(a1) :"0"(a1));
    }
};

#endif // ndef PRIMITIVE_OPS_POWERPC_H
