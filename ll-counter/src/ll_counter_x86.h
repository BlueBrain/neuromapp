#ifndef LL_COUNTER_X86_H
#define LL_COUNTER_X86_H

#include <cstdint>

struct x86_tsc {
    typedef uint64_t value_type;
    value_type read() {
        uint64_t v;
        asm volatile("rdtscp \n\t"
          : "=A"(v) :: "%rcx" );
        return v;
    }
    double counter_dt() const { return 0; }
    static constexpr bool is_serialized() { return true; }
};

#endif // ndef LL_COUNTER_X86_H

