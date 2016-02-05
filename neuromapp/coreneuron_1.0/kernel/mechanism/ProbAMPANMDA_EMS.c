/*
 * Neuromapp - ProbAMPANMDA_EMS.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * paramod.kumbhar@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/kernel/mechanism/ProbAMPANMDA_EMS.c
 * \brief Implementation of the ProbAMPANMDA_EMS kernels
 */

#include <stdlib.h>
#include <math.h>

#include "coreneuron_1.0/kernel/mechanism/mechanism.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/vectorizer.h"

/** stride for the SoA layout */
#define _STRIDE _cntml + _iml
/** time */
#define t _nt->_t
/** step time */
#define dt 0.001
/** Alias to get  */
#define tau_r_AMPA _p[0*_STRIDE]
#define tau_d_AMPA _p[1*_STRIDE]
#define tau_r_NMDA _p[2*_STRIDE]
#define tau_d_NMDA _p[3*_STRIDE]
#define Use _p[4*_STRIDE]
#define Dep _p[5*_STRIDE]
#define Fac _p[6*_STRIDE]
#define e _p[7*_STRIDE]
#define mg _p[8*_STRIDE]
#define u0 _p[9*_STRIDE]
#define synapseID _p[10*_STRIDE]
#define verboseLevel _p[11*_STRIDE]
#define NMDA_ratio _p[12*_STRIDE]
#define A_AMPA_step _p[13*_STRIDE]
#define B_AMPA_step _p[14*_STRIDE]
#define A_NMDA_step _p[15*_STRIDE]
#define B_NMDA_step _p[16*_STRIDE]
#define Rstate _p[17*_STRIDE]
#define tsyn_fac _p[18*_STRIDE]
#define u _p[19*_STRIDE]
#define A_AMPA _p[20*_STRIDE]
#define B_AMPA _p[21*_STRIDE]
#define A_NMDA _p[22*_STRIDE]
#define B_NMDA _p[23*_STRIDE]
#define i_AMPA _p[24*_STRIDE]
#define i_NMDA _p[25*_STRIDE]
#define g_NMDA _p[26*_STRIDE]
#define factor_AMPA _p[27*_STRIDE]
#define factor_NMDA _p[28*_STRIDE]
#define mggate _p[29*_STRIDE]
#define DA_AMPA _p[30*_STRIDE]
#define DB_AMPA _p[31*_STRIDE]
#define DA_NMDA _p[32*_STRIDE]
#define DB_NMDA _p[33*_STRIDE]
#define _v_unused _p[34*_STRIDE]
#define _g_unused _p[35*_STRIDE]
#define _tsav _p[36*_STRIDE]
#define _nd_area  _nt_data[_ppvar[0*_STRIDE]]
#define _p_rng  _nt->_vdata[_ppvar[2*_STRIDE]]

void mech_state_ProbAMPANMDA_EMS(NrnThread *_nt, Mechanism *_ml)
{
    int _cntml = _ml->nodecount;
    double * restrict _p = _ml->data;

    /* insert compiler dependent ivdep like pragma */
    _PRAGMA_FOR_VECTOR_LOOP_
    for (int _iml = 0; _iml < _cntml; ++_iml)
    {
        A_AMPA = A_AMPA * A_AMPA_step ;
        B_AMPA = B_AMPA * B_AMPA_step ;
        A_NMDA = A_NMDA * A_NMDA_step ;
        B_NMDA = B_NMDA * B_NMDA_step ;
    }
}

void mech_current_ProbAMPANMDA_EMS(NrnThread *_nt, Mechanism *_ml)
{
    double _rhs, _g = 0.0;
    int *_ni = _ml->nodeindices;
    int _cntml = _ml->nodecount;
    double * restrict _vec_rhs = _nt->_actual_rhs;
    double * restrict _vec_d = _nt->_actual_d;
    double * restrict _vec_shadow_rhs = _nt->_shadow_rhs;
    double * restrict _vec_shadow_d = _nt->_shadow_d;
    double * _nt_data = _nt->_data;
    double * restrict _vec_v = _nt->_actual_v;
    double * restrict _p = _ml->data;
    int *_ppvar = _ml->pdata;

    double gmax = 0.001;

    /* insert compiler dependent ivdep like pragma */
     _PRAGMA_FOR_VECTOR_LOOP_
    for (int _iml = 0; _iml < _cntml; ++_iml)
    {
        int _nd_idx = _ni[_iml];
        double _mfact =  1.e2/(_nd_area);
        double _lmggate , _lg_AMPA , _lg_NMDA , _lg , _li_AMPA , _li_NMDA , _lvv , _li , _lvve ;
        _lvv = _vec_v[_nd_idx];
        _lmggate = 1.0 / ( 1.0 + exp ( 0.062 * - ( _lvv ) ) * ( mg / 3.57 ) ) ;
        _lg_AMPA = gmax * ( B_AMPA - A_AMPA ) ;
        _lg_NMDA = gmax * ( B_NMDA - A_NMDA ) * _lmggate ;
        _lg = _lg_AMPA + _lg_NMDA ;
        _lvve = ( _lvv - e ) ;
        _li_AMPA = _lg_AMPA * _lvve ;
        _li_NMDA = _lg_NMDA * _lvve ;
        _li = _li_AMPA + _li_NMDA ;
        _rhs = _li;
        _g *=  _mfact;
        _rhs *= _mfact;

        _vec_shadow_rhs[_iml] = _rhs;
        _vec_shadow_d[_iml] = _g;
   }

    _PRAGMA_FOR_VECTOR_LOOP_
   for (int _iml = 0; _iml < _cntml; ++_iml)
   {
       int _nd_idx = _ni[_iml];
       _vec_rhs[_nd_idx] -= _vec_shadow_rhs[_iml];
       _vec_d[_nd_idx] += _vec_shadow_d[_iml];
   }
}

void mech_net_receive(NrnThread *_nt, Mechanism *_ml)
{
   int _iml = 0;
   int _cntml = _ml->nodecount;
   double* _p = _ml->data;
   double _args[5] = {0.21996815502643585, 0., 0., 0., 0.};
   double _lresult ;
   _args[1] = _args[0] ;
   _args[2] = _args[0] * NMDA_ratio ;
   if ( Fac > 0.0 ) {
     u = u * exp ( - ( t - tsyn_fac ) / Fac ) ;
     }
   else {
     u = Use ;
     }
   if ( Fac > 0.0 ) {
     u = u + Use * ( 1.0 - u ) ;
     }
   tsyn_fac = t ;
   if ( Rstate  == 0.0 ) {
     _args[3] = exp ( - ( t - _args[4] ) / Dep ) ;
     _lresult = 1.0 - (1.0 / (1.0 + _args[3]));
     if ( _lresult > _args[3] ) {
       Rstate = 1.0 ;
       }
     else {
       _args[4] = t ;
       }
     }
   if ( Rstate  == 1.0 ) {
     _lresult = 1.0 - (1.0 / (1.0 + u));
     if ( _lresult < u ) {
       _args[4] = t ;
       Rstate = 0.0 ;
       A_AMPA = A_AMPA + _args[1] * factor_AMPA ;
       B_AMPA = B_AMPA + _args[1] * factor_AMPA ;
       A_NMDA = A_NMDA + _args[2] * factor_NMDA ;
       B_NMDA = B_NMDA + _args[2] * factor_NMDA ;
       }
     }
}
