#ifndef LATENCY_KERNELS_H
#define LATENCY_KERNELS_H

/** Latency timing kernel with constant operation arguments
 *
 * Provides determination of latency of arithemtical operations
 * by inserting the operation in a dependent chain of instructions
 * within an inner-timing loop.
 *
 * Operands are reset to provided values maintaining a dependency
 * chain with the sequence:
 *
 *    a <- OP(a,b,...)
 *    a <- MUL(a,0)             # see below!
 *    a <- ADD(a,original_a)
 *
 * The dependency introduced by this sequence does rely upon the
 * CPU not reinterpreting the MUL as e.g. an immediate load.
 */

#include <cstddef>
#include "llc/unroll.h"
#include "llc/ll_common.h"

#include "primitive_ops.h"

/** Perform serially dependent sequence of arithmetic operations.
 *
 * @tparam V   Operand type (e.g. float, double)
 * @tparam op  Operation (e.g. arith_op::mul)
 * @tparam m   Number of 'op' operations
 * @tparam M   Maximum number of introduced 'op' operations.
 *
 * M is ignored in this kernel.
 */

template <typename V,arith_op::arith_op op,unsigned m,unsigned M>
struct kernel_dep_seq {
    /** Execute dep_seq kernel with given inner-loop count and operands.
     *
     * @param a1        Initial value of first operand to operation 'op'
     * @param a2        Second operand to operation 'op' 
     * @param a3        Third operand to operation 'op' 
     *
     */
    ALWAYS_INLINE static void run(V a1,V a2=0,V a3=0) {
        llc::ll_compiler_fence();
        ASM_LABEL("kernel_dep_seq begin");
        V b1=a1;
        llc::unroll<m>::run([&]() ALWAYS_INLINE_LAMBDA { primitive_op<op>::run(b1,a2,a3); });
        consume(b1);
        ASM_LABEL("kernel_dep_seq end");
        llc::ll_compiler_fence();
    }
};

/** Perform n_iter iterations of dep_seq sequence.
 *
 * @tparam V   Operand type (e.g. float, double)
 * @tparam op  Operation (e.g. arith_op::mul)
 * @tparam m   Number of 'op' operations
 * @tparam M   Maximum number of introduced 'op' operations.
 *
 * M is ignored in this kernel.
 */

template <typename V,arith_op::arith_op op,unsigned m,unsigned M>
struct kernel_looped_seq {
    /** Execute dep_seq kernel with given inner-loop count and operands.
     *
     * @param n_inner   Number of iterations of inner-loop
     * @param a1        Initial value of first operand to operation 'op'
     * @param a2        Second operand to operation 'op' 
     * @param a3        Third operand to operation 'op' 
     *
     */
    ALWAYS_INLINE static void run(size_t n_inner,V a1,V a2=0,V a3=0) {
        llc::ll_compiler_fence();
        ASM_LABEL("kernel_looped_seq begin");
        V b1=a1;
        for (unsigned i=0;i<n_inner;++i) {
            llc::unroll<m>::run([&]() ALWAYS_INLINE_LAMBDA { primitive_op<op>::run(b1,a2,a3); });
        }
        consume(b1);
        ASM_LABEL("kernel_looped_seq end");
        llc::ll_compiler_fence();
    }
};

/** Perform n_inner iterations of m arithmetic ops with constant operands.
 *
 * @tparam V   Operand type (e.g. float, double)
 * @tparam op  Operation (e.g. arith_op::mul)
 * @tparam m   Number of introduced 'op' operations within loop body
 * @tparam M   Maximum number of introduced 'op' operations.
 *
 * The parameter M is required so that we can provided constant overhead
 * for different values of m, taking into account the operations required
 * to reload the operands while maintaining a dependency chain.
 */

template <typename V,arith_op::arith_op op,unsigned m,unsigned M>
struct kernel_looped_karg {
    /** Execute looped_karg kernel with given inner-loop count and operands.
     *
     * @param n_inner   Number of iterations of inner-loop
     * @param a1        First operand to operation 'op'
     * @param a2        Second operand to operation 'op' 
     * @param a3        Third operand to operation 'op' 
     */
    ALWAYS_INLINE static void run(size_t n_inner,V a1,V a2=0,V a3=0) {
        llc::ll_compiler_fence();
        ASM_LABEL("kernel_looped_karg begin");
        V b1=a1;
        for (unsigned i=0;i<n_inner;++i) {
            ASM_LABEL("kernel_looped_karg op-seq");
            llc::unroll<m>::run([&]() ALWAYS_INLINE_LAMBDA {
                    primitive_op<op>::run(b1,a2,a3);
                    // reload b1 with data dependency
                    primitive_op<arith_op::sub>::run(b1,b1);
                    primitive_op<arith_op::add>::run(b1,a1);
                });
            ASM_LABEL("kernel_looped_karg no-op-seq");
            llc::unroll<M-m>::run([&]() ALWAYS_INLINE_LAMBDA {
                    // reload b1 with data dependency
                    primitive_op<arith_op::sub>::run(b1,b1);
                    primitive_op<arith_op::add>::run(b1,a1);
                });
        }
        consume(b1);
        ASM_LABEL("kernel_looped_karg end");
        llc::ll_compiler_fence();
    }
};

#endif // ndef LATENCY_KERNELS_H
