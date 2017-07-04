/*
 * Neuromapp - rng_utils.h, Copyright (c), 2015,
 * Francesco Casalegno - Swiss Federal Institute of technology in Lausanne,
 * francesco.casalegno@epfl.ch,
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
 * @file neuromapp/readi/rng_utils.h
 * \brief Random number genrator utilities for  Readi Miniapp
 */

#ifndef MAPP_READI_RNG_UTILS_
#define MAPP_READI_RNG_UTILS_



namespace readi {


// Random rounding of a floating point number.
// Returns floor(x) or ceil(x) by drawing a Bernoulli    
template <class IntType, class FloatType, class Generator>
IntType rand_round(FloatType x, Generator& g) {

    FloatType x_int = std::floor(x);
    std::bernoulli_distribution bd(x - x_int);
    return static_cast<IntType>(x_int)  + bd(g);

}




}

#endif// MAPP_READI_TETS_
