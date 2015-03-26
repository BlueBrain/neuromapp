#include <utility>

// don't use this with a function that takes rvalue refs; surprisingly bad
// results will ensue.

namespace llc {

#if __GNUC__==4 && __GNUC__MINOR__ >=8 || __GNUC__>=5
#define ALWAYS_INLINE [[gnu::always_inline]]
#else
#define ALWAYS_INLINE __attribute((always_inline))
#endif


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
    ALWAYS_INLINE static void run(...) {}
    ALWAYS_INLINE static void runi(...) {}
};

}
