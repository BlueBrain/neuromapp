/*
 * Neuromapp - memory.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * paramod.kumbhar@epfl.ch
 * All rights reserved.
 *
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

/**
 * @file neuromapp/coreneuron_1.0/common/memory/memory.h
 * \brief declaration function alignement/padding helper functions
 */

#define NRN_SOA_PAD 4 // here one AVX
#define NRN_SOA_BYTE_ALIGN 32

/** Independent function to compute the needed chunkding,
    the chunk argument is the number of doubles the chunk is chunkded upon.
*/
int soa_padded_size(int chunk, int cnt, int layout);

int nrn_soa_padded_size(int cnt, int layout);

/** Check for the pointer alignment.*/
int is_aligned(void* pointer, size_t alignment);
    
/** Allocate the aligned memory. */
void* emalloc_align(size_t size, size_t alignment);
    
/** Allocate the aligned memory and set it to 1.*/
void* ecalloc_align(size_t n, size_t alignment, size_t size);