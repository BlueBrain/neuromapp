#ifndef UNROLL_H
#define UNROLL_H

#include <utility>
#include "ll_common.h"

// don't use this with a function that takes rvalue refs; surprisingly bad
// results will ensue.

namespace llc {

template <unsigned N>
struct unroll {
    template <typename F,typename... Args>
    ALWAYS_INLINE static void run(F f,Args&&... args) {
        unroll<N-1>::run(f,std::forward<Args>(args)...);
        f(std::forward<Args>(args)...);
    }
    template <typename F,typename... Args>
    ALWAYS_INLINE static void runi(int i0,F f,Args&&... args) {
        unroll<N-1>::runi(i0,f,std::forward<Args>(args)...);
        f(i0+(N-1),std::forward<Args>(args)...);
    }
};

template <>
struct unroll<0> {
    template <typename... X>
    ALWAYS_INLINE static void run(X&& ...) {}
    template <typename... X>
    ALWAYS_INLINE static void runi(X&& ...) {}
};

} // namespace llc

#endif // ndef  UNROLL_H
