/*
 * Neuromapp - tools.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/keyvalue/utils/tools.h
 * \brief basic shell for mpi
 */

#ifndef MAPP_TOOLS_H
#define MAPP_TOOLS_H

#include <mpi.h>
#include <numeric>

namespace keyvalue {

    /** \fun accumulate(InputIt first, InputIt last, T init) 
    a kind of MPI accumulate
      it should be generic blabla, not type safe only work with DOUBLE */
    template<class InputIt, class T>
    inline T accumulate(InputIt first, InputIt last, T init){
        T tmp(0.), rec_mpi(0.);
        tmp  = std::accumulate(first,last, init);
        MPI_Reduce(&tmp, &rec_mpi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );
        return rec_mpi;
    }
    
} //end namespace keyvalue

#endif
