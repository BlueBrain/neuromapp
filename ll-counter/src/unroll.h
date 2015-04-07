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

template <unsigned N,template <int> class F,int i0=0>
struct unroll_static_n {
    // expands to F<i0>::run(args...); F<i0+1>::run(args...); ...
    template <typename... Args>
    ALWAYS_INLINE static void run(Args&&... args) {
        unroll_static_n<N-1,F,i0>::run(std::forward<Args>(args)...);
        F<i0+(N-1)>::run(std::forward<Args>(args)...);
    }
};

template <template <int> class F,int i0>
struct unroll_static_n<0,F,i0> {
    template <typename... X>
    ALWAYS_INLINE static void run(X&&...) {}
};

} // namespace llc

#endif // ndef  UNROLL_H
