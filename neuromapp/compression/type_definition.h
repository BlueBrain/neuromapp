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

struct memory_policy_cpu {
    typedef cstandard block_allocator_type;
    typedef zlib block_compressor_type;
    typedef double value_type;
};

typedef struct memory_policy_cpu memory_policy_type;

}; // namespace neuromapp
