#ifndef LL_COUNTER_H
#define LL_COUNTER_H

/** Low-level counter framework
 *
 * Wraps access to low-level counters, e.g. cycle counters,
 * op counters, time base values. etc.; uses required
 * synchronisation primitives to ameliorate re-ordering
 * issues at the compiler and CPU level.
 *
 * Supported compiler/archicture combinations:
 *     PowerPC: XL C++ 13.1
 *     x86: gcc 4.8, clang 3.5, icc (maybe?)
 */

#include <atomic>
#include <utility>

#if defined(__xlC)
#include <builtins.h>
#endif

#include "ll_common.h"

namespace llc {

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

static inline void ll_op_fence() {
#if defined(__powerpc__)
    asm volatile("isync" ::: "memory");
#elif defined(__i386__) || defined(__M_IX86) || defined(__x86_64__)
    asm volatile("lfence" ::: "memory");
#else
    #error "low level instruction barrier not implemented for architecture"
#endif
}

/* raw counter T interface:
 *
 * T(...)         counter initialisation
 * ~T()           release any required resources for counter
 * T::value_type  result type of T::read()
 * T::value_type read() const
 *                get counter value
 * static constexpr bool T::is_serialized()
 *                true if counter acquisition is
 *                serialized wrt preceding instructions
 * double counter_dt() const
 *                if applicable, return (estimate) of
 *                duration of one counter tick
 *
 * If counter can wrap, then T::value_type should
 * be an unsigned integral type, so that differences
 * can be safely taken with correct modulo behaviour
 */


/** Level of synchronisation imposed in
 * counter acquisition.
 */

namespace impl {
    // Synchronization guarantee policy classes

    /** Counter atomic store policy class
     *
     *     If sync is true, serialize memory access with respect
     *     to store of counter value of type T.
     */

    template <typename T,bool sync>
    struct sync_atomic {
        typedef T value_type;
        typedef T atomic_type;
        static void store(atomic_type &a,T v) { a=v; }
    };

    template <typename T>
    struct sync_atomic<T,true> {
        typedef T value_type;
        typedef std::atomic<value_type> atomic_type;
        static void store(atomic_type &a,T v) {
            (void)a.exchange(v,std::memory_order_acq_rel);
        }
    };

    /** Counter store instruction barrier policy class
     *
     *     If sync is true, fence() imposes a CPU instruction 
     *     barrier.
     */

    template <bool sync>
    struct sync_fence {
        static void fence() {}
    };

    template <>
    struct sync_fence<true> {
        static void fence() { ll_op_fence(); }
    };

} // namespace impl

/** Busy wait for fixed interval.
 *
 * Uses POSIX interval timer to interrupt a busy
 * wait loop.
 *
 * @param dt      Busy wait duration in seconds.
 * @param signal  Signal number for alarm (one-parameter version
 *                uses SIGRTMIN).
 * @return        Measured duratin.
 */

double busy_wait(double dt,int signal);
double busy_wait(double dt);

namespace ll_sync {
    enum ll_sync { none=0, memory=1, instruction=2 };
}

/** Higher-level interface to counter accumulation
 *
 * ll_counter accumulates differences in a counter value.
 * Initial state is 'paused', with zero accumulator.
 */

//template <typename cimpl,ll_sync::ll_sync sync=ll_sync::instruction>
template <typename cimpl,int sync=ll_sync::instruction>
struct ll_counter: protected cimpl {
    using typename cimpl::value_type;

protected:
    typedef impl::sync_fence<(ll_sync::instruction & sync)> fence_impl;
    typedef impl::sync_atomic<value_type,(ll_sync::memory & sync)> store_impl;
    typedef typename store_impl::atomic_type atomic_type;
    using cimpl::read;
    using cimpl::is_serialized;

public:
    template <typename... cimpl_arglist>
    ll_counter(cimpl_arglist&&... cimpl_args):
        cimpl(std::forward<cimpl_arglist>(cimpl_args)...) {}

    /** Reset accumulator, stop counting */
    void reset() {
        ll_compiler_fence();
        paused=true;
        accum=0;
        ll_compiler_fence();
    }

    /** Resume counting from paused state */
    void resume() {
        ll_compiler_fence();
        paused=false;
        read_counter(c0);
        ll_compiler_fence();
    }

    /** Pause counting */
    void stop() {
        ll_compiler_fence();
        read_counter(c1);
        accum+=c1-c0;
        paused=true;
        ll_compiler_fence();
    }

    /** Return accumulated counter difference
     *
     * Only valid when paused.
     */
    value_type count() {
        ll_compiler_fence();
        return accum;
    }

    void start() {
        reset();
        resume();
    }

    template <typename F,typename... Args>
    void instrument(const F &f,Args&&... args) {
        ll_compiler_fence();

        ASM_LABEL("instrument: c0 read");
        atomic_type local_c0;
        read_counter(local_c0);

        ll_compiler_fence();

        ASM_LABEL("instrument: kernel");
        f(std::forward<Args>(args)...);

        ll_compiler_fence();

        ASM_LABEL("instrument: c1 read");
        atomic_type local_c1;
        read_counter(local_c1);

        ll_compiler_fence();

        ASM_LABEL("instrument: counter accumulate");
        accum+=local_c1-local_c0;
    }

    struct guard {
        atomic_type guard_c0;
        ll_counter &llc;

        guard(ll_counter &llc_): llc(llc_) {
            ASM_LABEL("counter guard: c0 read");
            llc.read_counter(guard_c0);
        }
            
        ~guard() {
            ASM_LABEL("counter guard: c1 read");
            atomic_type guard_c1;
            llc.read_counter(guard_c1);

            ll_compiler_fence();

            ASM_LABEL("instrument_block: counter accumulate");
            llc.accum+=guard_c1-guard_c0;
        }
    };

protected:
    atomic_type c0,c1;
    value_type accum;
    bool paused;

    void read_counter(atomic_type &a) {
        if (!is_serialized()) fence_impl::fence();
        store_impl::store(a,read());
        fence_impl::fence();
    }
};


template <typename cimpl,ll_sync::ll_sync sync=ll_sync::instruction>
struct ll_timer: ll_counter<cimpl,sync> {
    template <typename... cimpl_arglist>
    ll_timer(cimpl_arglist&&... cimpl_args):
        ll_counter<cimpl,sync>(std::forward<cimpl_arglist>(cimpl_args)...), dt(this->counter_dt()) {}

    double elapsed() {
        return (double)(this->count())*dt;
    }

    /** Re-query counter's dt value.
     * 
     * Set dt to value reported by counter, or if
     * this is zero (i.e. no known tick duration)
     * optionally call estimate_dt to busy wait
     * for a fixed interval.*/
    
    void update_dt(double estimate_ival=0) {
        double dt_=this->counter_dt();
        if (dt_==0 && estimate_ival>0) {
            estimate_dt(estimate_ival);
        }
        else dt=dt_;
    }
    
    /** Estimate counter time step with busy wait.
     *
     * @param estimate_ival   busy wait duration in seconds
     */

    void estimate_dt(double estimate_ival) {
        double dt_;
        this->start();
        dt_=busy_wait(estimate_ival);
        this->stop();
        dt=dt_/this->count();
    }

    double dt;
};

} // namespace llc

#endif // ndef LL_COUNTER_H
