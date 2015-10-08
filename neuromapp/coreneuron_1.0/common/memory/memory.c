/*
 * Neuromapp - memory.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * paramod.kumbhar@epfl.ch
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/common/util/memory.h
 * Implements function alignement/padding helper functions
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "coreneuron_1.0/common/memory/memory.h"

/** Independent function to compute the needed chunkding,
    the chunk argument is the number of doubles the chunk is chunkded upon. */
inline int soa_padded_size(int chunk, int cnt, int layout) {
    int imod = cnt % chunk;
    if (layout == 1) return cnt;
    if (imod) {
        int idiv = cnt / chunk;
        return (idiv + 1) * chunk;
    }
    return cnt;
}

int inline nrn_soa_padded_size(int cnt, int layout) {
    return soa_padded_size(NRN_SOA_PAD,cnt, layout);
}

/** Check for the pointer alignment.*/
inline int is_aligned(void* pointer, size_t alignment) {
    return (((uintptr_t)(const void *)(pointer)) % (alignment) == 0);
}

/** Allocate the aligned memory. */
inline void* emalloc_align(size_t size, size_t alignment) {
    void* memptr;
    int b1 = posix_memalign(&memptr, alignment, size);
    int b2 = is_aligned(memptr, alignment);
    /* Debug mode only */
    assert(b1 == 0);
    assert(b2 == 1);
    return memptr;
}

/** Allocate the aligned memory and set it to 0.*/
inline void* ecalloc_align(size_t n, size_t alignment, size_t size) {
    void* p;
    if (n == 0) { return (void*)0; }
    int b1 = posix_memalign( &p, alignment, n*size);
    int b2 = is_aligned(p, alignment);
    /* Debug mode only */
    assert(b1 == 0);
    assert(b2 == 1);
    memset(p, 0, n*size);
    return p;
}
