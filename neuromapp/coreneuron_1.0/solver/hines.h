

/**
 * @file neuromapp/coreneuron_1.0/solver/hines.h
 * Implements the Hines solver
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