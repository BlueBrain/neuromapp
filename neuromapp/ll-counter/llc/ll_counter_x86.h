#ifndef LL_COUNTER_X86_H
#define LL_COUNTER_X86_H

#include <cstdint>

struct x86_tsc {
    typedef uint64_t value_type;
    value_type read() {
        uint64_t v;
        asm volatile("rdtscp;"
                     "shl $32, %%rdx;"
                     "or %%rdx ,%%rax;"
          : "=A"(v) : : "%rcx", "%rdx" );
        return v;
    }
    double counter_dt() const { return 0; }
    static constexpr bool is_serialized() { return true; }
};

#endif // ndef LL_COUNTER_X86_H

