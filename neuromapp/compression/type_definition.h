/*
 * Neuromapp - type_definition.h, Copyright (c), 2015,
 * All rights reserved.
 *
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
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

} // namespace neuromapp
