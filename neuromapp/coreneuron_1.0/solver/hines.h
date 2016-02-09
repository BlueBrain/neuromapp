/*
 * Neuromapp - hines.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * francesco.cremonesi@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/solver/hines.h
 * \brief Implements the Hines solver
 */

#ifndef MAPP_SOLVER_HINES_
#define MAPP_SOLVER_HINES_

#include "coreneuron_1.0/common/memory/nrnthread.h"

#ifdef __cplusplus
    extern "C" {
        /** \fn void nrn_solve_minimal(NrnThread* _nt)
            \brief solve the matrix equation
            \param NrnThread the data structure for access to the matrix data
         */
        void nrn_solve_minimal(NrnThread* _nt);

        /** \fn void triang(NrnThread* _nt)
            \brief triangularization of the matrix equations
            \param NrnThread the data structure for access to the matrix data
         */
        void triang(NrnThread*);

        /** \fn void bksub(NrnThread* _nt)
            \brief back substitution to finish solving the matrix equations
            \param NrnThread the data structure for access to the matrix data
         */
        void bksub(NrnThread*);
    }
#else
    /** \fn void nrn_solve_minimal(NrnThread* _nt)
        \brief solve the matrix equation
        \param NrnThread the data structure for access to the matrix data
    */
    void nrn_solve_minimal(NrnThread* _nt);

    /** \fn void triang(NrnThread* _nt)
        \brief triangularization of the matrix equations
        \param NrnThread the data structure for access to the matrix data
     */
    void triang(NrnThread*);

    /** \fn void bksub(NrnThread* _nt)
        \brief back substitution to finish solving the matrix equations
        \param NrnThread the data structure for access to the matrix data
     */
    void bksub(NrnThread*);
#endif

#endif