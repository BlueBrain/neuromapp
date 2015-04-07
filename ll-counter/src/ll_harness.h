#ifndef LL_HARNESS_H
#define LL_HARNESS_H

#include <cstddef>
#include <cstdint>
#include <limits>

#include "ll_common.h"
#include "ll_counter.h"

namespace llc {

// perhaps replace with approximate online quantile?
template <typename T,typename fp_type=double>
struct qstats {
    typedef T value_type;
    typedef fp_type real_type;
    typedef size_t size_type;

    qstats() { clear(); }

    void clear() {
        n=0; 
        if (std::numeric_limits<T>::has_infinity) {
            minv=std::numeric_limits<T>::infinity();
            maxv=-minv;

        }
        else {
            minv=std::numeric_limits<T>::max();
            maxv=std::numeric_limits<T>::lowest();
        }
        sum=0;
    }
    
    void push(value_type v) {
        minv=v<minv?v:minv;
        maxv=v>maxv?v:maxv;
        sum+=static_cast<real_type>(v);
        ++n;
    }

    value_type min() const { return minv; }
    value_type max() const { return maxv; }
    size_t size() const { return n; }
    real_type mean() const { return sum/static_cast<real_type>(n); }

protected:
    value_type minv,maxv;
    real_type sum;
    size_t n;
};

// Note: using compile-time values for iteration count at the moment,
// to support fixed-size allocation of a results buffer or similar.
// Current version of qstats though does not care, so we could use
// an invocation- or construction-time value instead.

template <typename counter_type,size_t n_iter_,size_t n_warmup_=3,bool instrument_harness=true>
struct ll_harness {
    constexpr static size_t n_iter=n_iter_;
    constexpr static size_t n_warmup=n_warmup_;

    typedef typename counter_type::value_type value_type;

    template <typename... cimpl_arglist>
    explicit ll_harness(cimpl_arglist&&... cimpl_args):
        C(std::forward<cimpl_arglist>(cimpl_args)...) {}


    void clear() {
        run_stats.clear();
        harness_stats.clear();
    }

    template <typename F,typename... Args>
    void run(F&& f,Args&&... args) {
        clear();
        run_harness();

        // collect stats on F
        ASM_LABEL("harness: warmup kernel loop");
        for (size_t i=0;i<n_warmup;++i) {
            counter_guard g(C);
            f(std::forward<Args>(args)...);
        }
        ASM_LABEL("harness: timed kernel loop");
        for (size_t i=0;i<n_iter;++i) {
            C.reset();
            {
                counter_guard g(C);
                f(std::forward<Args>(args)...);
            }
            run_stats.push(C.count());
        }
    }
    
    // Try a function-only specialization in an attempt (successful!)
    // to avoid inline failures.
 
    template <void (F)()>
    void run() {
        clear();
        run_harness();

        // collect stats on F
        ASM_LABEL("harness: warmup kernel loop");
        for (size_t i=0;i<n_warmup;++i) {
            counter_guard g(C);
            F();
        }

        ASM_LABEL("harness: timed kernel loop");
        for (size_t i=0;i<n_iter;++i) {
            C.reset();
            {
                counter_guard g(C);
                F();
            }
            run_stats.push(C.count());
        }
    }
    value_type min_count() const { return run_stats.min(); }
    value_type min_count_harness() const { return instrument_harness?harness_stats.min():0; }

protected:
    void run_harness() {
        if (instrument_harness) {
            // estimate harness overhead
            C.stop();
            ASM_LABEL("harness: warmup counter-only loop");
            for (size_t i=0;i<n_warmup;++i) {
                counter_guard g(C);
                ll_compiler_fence();
            }
            ASM_LABEL("harness: timed counter-only loop");
            for (size_t i=0;i<n_iter;++i) {
                C.reset();
                {
                    counter_guard g(C);
                    ll_compiler_fence();
                }
                harness_stats.push(C.count());
            }
        }
    }

    counter_type C;
    typedef typename counter_type::guard counter_guard;
    qstats<value_type> run_stats,harness_stats;
};

} // namespace llc

#endif // ndef LL_HARNESS_H
