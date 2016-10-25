/*
 * Neuromapp - foreach.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/utils/omp/foreach.h
 * \brief Contains paralle foreach implementation.
 */

#ifndef MAPP_FOREACH_H_
#define MAPP_FOREACH_H_

#ifdef _OPENMP
#include <omp.h>

namespace queueing {

template<class InputUt, class UnaryFunction>
UnaryFunction for_each(InputIt First, InputIt last, UnaryFunction f){
    #pragma omp parallel for
    for(; first != last; ++ first) {
        f(*first);
    }
    return f;

}


}
#endif

#endif
