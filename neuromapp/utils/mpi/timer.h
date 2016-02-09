/*
 * Neuromapp - timer, Copyright (c), 2015,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee ewart - Swiss Federal Institute of technology in Lausanne,
 * sam.yates@epfl.ch (work)
 * timothee.ewart@epfl.ch (idea)
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
 * @file neuromapp/utils/mpi/timer.h
 * \brief tiny timer
 */


#ifndef MAPP_TIMER_H
#define MAPP_TIMER_H

#include <mpi.h>

namespace mapp{
    /** a tiny timer only MPI now */
    class timer{
    public:
        explicit timer(double t0 = 0.):t(t0){}

        inline void tic(){
            t = MPI_Wtime();
        }

        inline void toc(){
            t = MPI_Wtime() - t;
        }

        inline double time() {
            return t;
        }

    private:
        double t;
    };
} // end namespace
#endif