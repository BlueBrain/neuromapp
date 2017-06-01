#pragma once

#pragma once

#include <cassert>
#include <ostream>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace neuromapp {



// forward
class align;
class cstandard;
class zlib;

struct compressor_policy {
    // this essentially means using the utility compress and 
    // decompress algorithms instead of the
    // inflate deflate lower level functions from the zlib
    typedef zlib zlib_util_algos;
};

struct memory_policy_cpu {
    typedef align block_type;
};

typedef struct memory_policy_cpu memory_policy_type;

struct definition {
    typedef double value_type;
};
}; // namespace neuromapp
