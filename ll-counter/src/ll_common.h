#ifndef LL_COMMON_H_
#define LL_COMMON_H_

/** Common defines, utilities for llc library. */

// #define LL_COMMON_STRINGIFY_(x) #x

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

/** Insert comment in emmited assembly with fence.
 *
 * @param label Label: must be a string literal
 *
 * Include a label as for asm_label(), but also
 * enforce a compiler fence.
 */

#define ASM_LABEL_FENCE(label) \
asm ("#" label "\n\t" ::: "memory")


/** Force inline macro */

#if __GNUC__==4 && __GNUC__MINOR__ >=8 || __GNUC__>=5
#define ALWAYS_INLINE [[gnu::always_inline]]
#else
#define ALWAYS_INLINE __attribute((always_inline))
#endif


namespace llc {

/** Direct compiler not to reorder instructions
 * across fence.
 */
static inline void ll_compiler_fence() {
    asm volatile("" ::: "memory");
}

} // namespace llc

#endif // ndef LL_COMMON_H_
