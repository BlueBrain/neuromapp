#ifndef LL_COUNTER_POWER8_H
#define LL_COUNTER_POWER8_H

#include <cstdint>
#include <stdexcept>
#include <limits>
#include <memory>

#include "ll_counter.h"
#include "linux_perf_count.h"

namespace llc {

template <int spr>
static inline uint64_t read_spr() {
    uint64_t r;
    asm volatile("mfspr %[r],%[spr] \n\t"
      : [r]"=r"(r) : [spr]"i"(spr) : "memory");
    return r;
}

template <int spr>
static inline void write_spr(uint64_t v) {
    asm volatile("mtspr %[spr],%[r] \n\t"
      : : [r]"r"(v), [spr]"i"(spr) : "memory");
}

/** Generic counter implementation around an SPR */

template <int spr>
struct ppc_spr {
    typedef uint64_t value_type;
    value_type read() const { return read_spr<spr>(); }
    void write(value_type v) const { return write_spr<spr>(v); }
    double counter_dt() const { return 0; }
    static constexpr bool is_serialized() { return false; }
};

/** Traslates Power ISA bit-field descriptions
 *  (which are MSB0 order) into 64-bit masks.
 *
 * @param msb Most significant bit number in MSB0 order
 * @param lsb Most significant bit number in MSB0 order
 *
 * Note lsb >= msb in this representation.
 */

template <typename UINT,int msb,int lsb=msb>
struct msb0_bitfield {
    static constexpr unsigned word_bits=std::numeric_limits<UINT>::digits;
    static constexpr unsigned bits=lsb-msb+1;
    static constexpr unsigned shift=word_bits-lsb-1;

    static constexpr UINT mask=(bits==word_bits)?(UINT)-1:((UINT(1)<<bits)-1)<<shift;
};

template <int msb,int lsb=msb>
using msb0_bitfield64 = msb0_bitfield<uint64_t,msb,lsb>;

/*
constexpr uint64_t mask_msb0(int bit) {
    return 1ull<<(63-bit);
}

constexpr uint64_t mask_msb0(int msb,int lsb) {
    return (lsb==63 && msb==0)?(uint64_t)-1:((1ull<<(1+lsb-msb))-1)<<(63-lsb);
}
*/


namespace spr {
    enum reg: int {
        mmcr0=779,
        mmcr1=782,
        mmcr2=769,
        pmc1=771,
        pmc2=772,
        pmc3=773,
        pmc4=774,
        pmc5=775,
        pmc6=776,
        tb=268
    };

/*
    enum mask: uint64_t {
        mmcr0_fc  =mask_msb0(32),
        mmcr0_pmcc=mask_msb0(44,45),
        mmcr0_fc14=mask_msb0(58),
        mmcr0_fc56=mask_msb0(59)
    };
*/

    typedef msb0_bitfield64<32>    mmcr0_fc;
    typedef msb0_bitfield64<44,45> mmcr0_pmcc;
    typedef msb0_bitfield64<58>    mmcr0_fc14;
    typedef msb0_bitfield64<59>    mmcr0_fc56;
};

inline bool pmc56_running() {
    ll_op_fence();
    uint64_t mmcr0=read_spr<spr::mmcr0>();
    //return !(mmcr0 & (spr::mmcr0_fc | spr::mmcr0_fc56)) && !((mmcr0 & spr::mmcr0_pmcc) == spr::mmcr0_pmcc);

    return !(mmcr0 & (spr::mmcr0_fc::mask | spr::mmcr0_fc56::mask)) &&
           ((mmcr0 & spr::mmcr0_pmcc::mask) >> spr::mmcr0_pmcc::shift) != 3;
}

struct ppc_cycle: ppc_spr<spr::pmc6> {
    std::shared_ptr<perf_event_set> P;

    ppc_cycle() {
        if (!pmc56_running()) {
            // ask kernel to enable PMC6 through registering a raw
            // performance event
            P.reset(new perf_event_set(perf_event_raw(0x600f4)));

            if (!pmc56_running())
                throw std::runtime_error("PMC5/6 unavailable");
        }
    }
};

struct ppc_op: ppc_spr<spr::pmc5> {
    std::shared_ptr<perf_event_set> P;

    ppc_op() {
        if (!pmc56_running()) {
            // ask kernel to enable PMC5 through registering a raw
            // performance event
            P.reset(new perf_event_set(perf_event_raw(0x500fa)));

            if (!pmc56_running())
                throw std::runtime_error("PMC5/6 unavailable");
        }
    }
};


struct ppc_time_base: ppc_spr<spr::tb> {
    double counter_dt() const { return 1/512.e6; }
};

// TODO: add atb SPR

#if 0
struct ppc_op {
    typedef uint64_t value_type;
    value_type read() {
        uint64_t v;
        asm volatile("mfspr %0,775 \n\t"
          : "=r"(v));
        return v;
    }
    double counter_dt() const { return 0; }
    static constexpr bool is_serialized() { return false; }
};

struct ppc_time_base {
    typedef uint64_t value_type;
    value_type read() {
        uint64_t v;
        asm volatile("mfspr %0,268 \n\t"
          : "=r"(v));
        return v;
    }
    double counter_dt() const { return 0; }
    static constexpr bool is_serialized() { return false; }
};
#endif

} // namespace llc

#endif // ndef LL_COUNTER_POWER8_H

