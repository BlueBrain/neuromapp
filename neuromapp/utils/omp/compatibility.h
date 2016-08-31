/*
 * Neuromapp - compatibility.h, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * @file neuromapp/utils/omp/compatibility.h
 * \brief Contains support for OMP functions when OMP is not available.
 */

#ifndef MAPP_COMP_H_
#define MAPP_COMP_H_

#ifdef _OPENMP
// If OpenMP is available, include the header
#include <omp.h>
#else
// Otherwise, define dummy functions so that the mini-apps work properly
#include <iostream>

inline int omp_get_num_threads() { return 1; }
inline int omp_get_thread_num() { return 0; }
void omp_set_num_threads (int threads)
{
    if (threads > 1)
        std::cout << "Setting the number of OMP threads, but OMP is not available. Execution may be wrong!" << std::endl;
}

#endif

#endif // MAPP_COMP_H_
