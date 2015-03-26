#ifndef LL_COUNTER_GENERIC_PERF_H
#define LL_COUNTER_GENERIC_PERF_H

#include <cstdint>

#include "linux_perf_count.h"

namespace llc {

struct generic_perf {
    std::shared_ptr<perf_event_set> P;

    generic_perf(perf_event ev): P(new perf_event_set(ev)) {}

    typedef uint64_t value_type;
    value_type read() {
        P->sample();
        return (*P)[0];
    }
    double counter_dt() const { return 0; }
    static constexpr bool is_serialized() { return true; }
};

}

#endif // ndef LL_COUNTER_GENERIC_PERF_H

