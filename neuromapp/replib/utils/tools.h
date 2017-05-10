/*
 * Neuromapp - tools.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
 *
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
 * @file neuromapp/replib/utils/tools.h
 * \brief basic shell for mpi and report checking
 */

#ifndef MAPP_TOOLS_H
#define MAPP_TOOLS_H

#include <mpi.h>
#include <numeric>
#include <cmath>


namespace replib {

    /** \fun reduce(T & value)
    returns the reduction of the value of each rank
    it should be generic blabla, not type safe only work with DOUBLE */
    template<class T>
    inline T reduce(T & value) {
        T red_mpi = 0.0;
        MPI_Reduce(&value, &red_mpi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        return red_mpi;
    }

    /** \fun max(const T in, T out, int rank)
    find the max value and its corresponding rank
    it should be generic blabla, not type safe only work with DOUBLE */
    template<class T>
    void max(const T in, T out, int rank) {
             MPI_Reduce(in, out, 1, MPI_DOUBLE_INT, MPI_MAXLOC, rank, MPI_COMM_WORLD);
    }

    /** \fun min(const T in, T out, int rank)
    find the min value and its corresponding rank
    it should be generic blabla, not type safe only work with DOUBLE */
    template<class T>
    void min(const T in, T out, int rank) {
             MPI_Reduce(in, out, 1, MPI_DOUBLE_INT, MPI_MINLOC, rank, MPI_COMM_WORLD);
    }

    /** \fun check_report(char * report, int nwrites, int repCycleElems, int mpiSize, float units, int decimals)
    check the written report for correctness.
    Returns true if the checking succeeded */
    bool check_report (char * report, int nwrites, int repCycleElems, int mpiSize, float units, int decimals);

} //end namespace replib

#endif
