#ifndef CONV_INFO_H
#define CONV_INFO_H
#include <cstdint>

template <typename T>
struct Conv_info {};

template <>
struct Conv_info<float> {
    static const int sign_size = 1;
    static const int exp_size = 8;
    static const int mant_size = 23;
    static const int total = sign_size + exp_size + mant_size;
    typedef uint32_t bytetype;
};


template <>
struct Conv_info<double> {
    static const int sign_size = 1;
    static const int exp_size = 11;
    static const int mant_size = 52;
    static const int total = sign_size + exp_size + mant_size;
    typedef uint64_t bytetype;
};

#endif
