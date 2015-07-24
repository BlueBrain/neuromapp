#ifndef PRIMITIVE_OPS_POWER8_H
#define PRIMITIVE_OPS_POWER8_H

/** Implementation of primitive ops for POWER8 (or Power ISA v2.07).
 *
 *  Should be included automatically from primitive_ops.h and not directly by
 *  other code.
 *
 *  NB: Use of "wy", as opposed to "wf" or "wd", asm constraint for vector
 *  operations is required to ensure correct adherence to ABI (or even
 *  correct loading of registers at all when inlined.) It is not clear why
 *  this should be so.
 */


template <>
struct primitive_op<arith_op::add> {
    ALWAYS_INLINE static void run(float &a1,float a2,...) {
        asm volatile ("fadds %0,%0,%1\n\t" :"+f"(a1) :"f"(a2));
    }
    ALWAYS_INLINE static void run(double &a1,double a2,...) {
        asm volatile ("fadd %0,%0,%1\n\t" :"+d"(a1) :"d"(a2));
    }
    ALWAYS_INLINE static void run(v4float &a1,v4float a2,...) {
        asm volatile ("xvaddsp %0,%0,%1\n\t" :"+wy"(a1) :"wy"(a2));
    }
    ALWAYS_INLINE static void run(v2double &a1,v2double &a2,...) {
        asm volatile ("xvadddp %0,%0,%1\n\t" :"+wy"(a1) :"wy"(a2));
    }
    static constexpr bool is_specialized=true;
};

template <>
struct primitive_op<arith_op::sub> {
    ALWAYS_INLINE static void run(float &a1,float a2,...) {
        asm volatile ("fsubs %0,%0,%1\n\t" :"+f"(a1) :"f"(a2));
    }
    ALWAYS_INLINE static void run(double &a1,double a2,...) {
        asm volatile ("fsub %0,%0,%1\n\t" :"+d"(a1) :"d"(a2));
    }
    ALWAYS_INLINE static void run(v4float &a1,v4float a2,...) {
        asm volatile ("xvsubsp %0,%0,%1\n\t" :"+wy"(a1) :"wy"(a2));
    }
    ALWAYS_INLINE static void run(v2double &a1,v2double a2,...) {
        asm volatile ("xvsubdp %0,%0,%1\n\t" :"+wy"(a1) :"wy"(a2));
    }
    static constexpr bool is_specialized=true;
};

template <>
struct primitive_op<arith_op::mul> {
    ALWAYS_INLINE static void run(float &a1,float a2,...) {
        asm volatile ("fmuls %0,%0,%1\n\t" :"+f"(a1) :"f"(a2));
    }
    ALWAYS_INLINE static void run(double &a1,double a2,...) {
        asm volatile ("fmul %0,%0,%1\n\t" :"+d"(a1) :"d"(a2));
    }
    ALWAYS_INLINE static void run(v4float &a1,v4float a2,...) {
        asm volatile ("xvmulsp %0,%0,%1\n\t" :"+wy"(a1) :"wy"(a2));
    }
    ALWAYS_INLINE static void run(v2double &a1,v2double a2,...) {
        asm volatile ("xvmuldp %0,%0,%1\n\t" :"+wy"(a1) :"wy"(a2));
    }
    static constexpr bool is_specialized=true;
};

template <>
struct primitive_op<arith_op::div> {
    ALWAYS_INLINE static void run(float &a1,float a2,...) {
        asm volatile ("fdivs %0,%0,%1\n\t" :"+f"(a1) :"f"(a2));
    }
    ALWAYS_INLINE static void run(double &a1,double a2,...) {
        asm volatile ("fdiv %0,%0,%1\n\t" :"+d"(a1) :"d"(a2));
    }
    ALWAYS_INLINE static void run(v4float &a1,v4float a2,...) {
        asm volatile ("xvdivsp %0,%0,%1\n\t" :"+wy"(a1) :"wy"(a2));
    }
    ALWAYS_INLINE static void run(v2double &a1,v2double a2,...) {
        asm volatile ("xvdivdp %0,%0,%1\n\t" :"+wy"(a1) :"wy"(a2));
    }
    static constexpr bool is_specialized=true;
};

template <>
struct primitive_op<arith_op::fma> {
    ALWAYS_INLINE static void run(float &a1,float a2,float a3,...) {
        asm volatile ("fmadds %0,%0,%1,%2\n\t" :"+f"(a1) :"f"(a2),"f"(a3));
    }
    ALWAYS_INLINE static void run(double &a1,double a2,double a3,...) {
        asm volatile ("fmadd %0,%0,%1,%2\n\t" :"+d"(a1) :"d"(a2),"d"(a3));
    }
    ALWAYS_INLINE static void run(v4float &a1,v4float a2,v4float a3,...) {
        asm volatile ("xvmaddmsp %0,%1,%2\n\t" :"+wy"(a1) :"wy"(a2),"wy"(a3));
    }
    ALWAYS_INLINE static void run(v2double &a1,v2double a2,v2double a3,...) {
        asm volatile ("xvmaddmdp %0,%1,%2\n\t" :"+wy"(a1) :"wy"(a2),"wy"(a3));
    }
    static constexpr bool is_specialized=true;
};

template <>
struct primitive_op<arith_op::sqrt> {
    ALWAYS_INLINE static void run(float &a1,...) {
        asm volatile ("xssqrtsp %0,%0\n\t" :"+wy"(a1));
    }
    ALWAYS_INLINE static void run(double &a1,...) {
        asm volatile ("xssqrtdp %0,%0\n\t" :"+wy"(a1));
    }
    ALWAYS_INLINE static void run(v4float &a1,...) {
        asm volatile ("xvsqrtsp %0,%0\n\t" :"+wy"(a1));
    }
    ALWAYS_INLINE static void run(v2double &a1...) {
        asm volatile ("xvsqrtdp %0,%0\n\t" :"+wy"(a1));
    }
    static constexpr bool is_specialized=true;
};

inline void consume(float v)  { asm volatile ("" ::"f"(v)); }
inline void consume(double v) { asm volatile ("" ::"d"(v)); }
inline void consume(v4float v) { asm volatile ("" ::"wy"(v)); }
inline void consume(v2double v) { asm volatile ("" ::"wy"(v)); }

#endif // ndef PRIMITIVE_OPS_POWER8_H
