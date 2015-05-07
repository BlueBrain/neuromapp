#ifndef LL_COMMON_H_
#define LL_COMMON_H_

/** Common defines, utilities for llc library. */

/** Insert comment in emitted assembly in-place.
 *
 * @param label Label: must be a string literal
 *
 * Note that this is (implicitly) asm volatile,
 * and will break any optimizations that rely on
 * being able to elide this statement.
 */

#define ASM_LABEL(label) \
asm ("#" label "\n\t")

/** Insert comment in emitted assembly with fence.
 *
 * @param label Label: must be a string literal
 *
 * Include a label as for asm_label(), but also
 * enforce a compiler fence.
 */

#define ASM_LABEL_FENCE(label) \
asm ("#" label "\n\t" ::: "memory")

/** Force inline macro
 *
 * [[gnu::always_inline]] C++11 extended attribute
 * syntax parsed and honoured by gcc version >=4.8
 * and clang >= 3.3.
 *
 * __attribute((always_inline)) supported by earlier
 * versions of gcc, and many other compilers, but is
 * a non-standard extension.
 */

#if (__GNUC__>=5 || __GNUC__==4 && __GNUC__MINOR__ >=8) \
 || (__clang_major__>=4 || __clang_major__==3 && __clang_minor__>=3)
#define ALWAYS_INLINE [[gnu::always_inline]] inline 
#define ALWAYS_INLINE_LAMBDA [[gnu::always_inline]]
#else
#define ALWAYS_INLINE __attribute((always_inline)) inline
#define ALWAYS_INLINE_LAMBDA __attribute((always_inline))
#endif

namespace llc {

/** Direct compiler not to reorder instructions
 * across fence.
 */
ALWAYS_INLINE static void ll_compiler_fence() {
    asm volatile("" ::: "memory");
}

/** Direct cpu to complete all prior instructions to the
 * barrier, and not begin any subsequent instructions until
 * after the barrier.
 *
 * PowerPC implementation:
 *     isync
 * see: Power ISA 2.07 p.776.

 * x86 implementation:
 *     lfence
 * see: Intel 64 and IA-32 Archictecture Software Developer's Manual
 *      Vol3A, January 2015; Note 4, p. 8-17.
 */

ALWAYS_INLINE static void ll_op_fence() {
#if defined(__powerpc__)
    asm volatile("isync" ::: "memory");
#elif defined(__i386__) || defined(__M_IX86) || defined(__x86_64__)
    asm volatile("lfence" ::: "memory");
#else
    #error "low level instruction barrier not implemented for architecture"
#endif
}

} // namespace llc

#endif // ndef LL_COMMON_H_
