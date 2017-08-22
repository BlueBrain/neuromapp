#ifndef CONV_INFO_H
#define CONV_INFO_H
#include <cstdint>

template <typename T>
struct Conv_info {};

template <>
struct Conv_info<float> {
    static const uint32_t sign_size = 1;
    static const uint32_t exp_size = 8;
    static const uint32_t mant_size = 23;
    static const uint32_t total = sign_size + exp_size + mant_size;
    typedef uint32_t bytetype;
};


template <>
struct Conv_info<double> {
    static const uint64_t sign_size = 1;
    static const uint64_t exp_size = 11;
    static const uint64_t mant_size = 52;
    static const uint64_t total = sign_size + exp_size + mant_size;
    typedef uint64_t bytetype;
};

#endif
