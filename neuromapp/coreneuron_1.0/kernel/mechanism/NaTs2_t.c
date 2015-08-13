/*
 * Neuromapp - NaTs2_t.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * paramod.kumbhar@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/kernel/mechanism/NaTs2_t.c
 * Implementation of the NaTs2_ kernels
 */

#include <math.h>

#include "coreneuron_1.0/kernel/mechanism/mechanism.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/vectorizer.h"

#define _STRIDE _cntml + _iml
#define t _nt->_t
#define dt 0.001

#define gNaTs2_tbar _p[0*_STRIDE]
#define m _p[1*_STRIDE]
#define h _p[2*_STRIDE]
#define ena _p[3*_STRIDE]
#define Dm _p[4*_STRIDE]
#define Dh _p[5*_STRIDE]
#define _v_unused _p[6*_STRIDE]
#define _g_unused _p[7*_STRIDE]
#define _ion_ena _nt_data[_ppvar[0*_STRIDE]]
#define _ion_ina _nt_data[_ppvar[1*_STRIDE]]
#define _ion_dinadv _nt_data[_ppvar[2*_STRIDE]]

void mech_init_NaTs2_t(NrnThread *_nt, Mechanism *_ml)
{
    double _v, v;
    int *_ni = _ml->nodeindices;
    int _cntml = _ml->nodecount;
    double * restrict _p = _ml->data;
    int * restrict _ppvar = _ml->pdata;
    double * restrict _vec_v = _nt->_actual_v;
    double * restrict _nt_data = _nt->_data;

    /* insert compiler dependent ivdep like pragma */
    _PRAGMA_FOR_VECTOR_LOOP_
    for (int _iml = 0; _iml < _cntml; ++_iml)
    {
        int _nd_idx = _ni[_iml];
        _v = _vec_v[_nd_idx];
        v=_v;
        ena = _ion_ena;
        double _lmAlpha , _lmBeta , _lmInf , _lmTau , _lhAlpha , _lhBeta , _lhInf , _lhTau , _llv=0.0;
        double _lqt=2.952882641412121 ;

        _llv = v;
        if ( _llv  == - 32.0 )
            _llv = _llv + 0.0001 ;

        _lmAlpha = ( 0.182 * ( _llv - - 32.0 ) ) / ( 1.0 - ( exp ( - ( _llv - - 32.0 ) / 6.0 ) ) ) ;
        _lmBeta = ( 0.124 * ( - _llv - 32.0 ) ) / ( 1.0 - ( exp ( - ( - _llv - 32.0 ) / 6.0 ) ) ) ;
        _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
        _lmTau = ( 1.0 / ( _lmAlpha + _lmBeta ) ) / _lqt ;

        if ( _llv  == - 60.0 )
          _llv = _llv + 0.0001 ;

        _lhAlpha = ( - 0.015 * ( _llv - - 60.0 ) ) / ( 1.0 - ( exp ( ( _llv - - 60.0 ) / 6.0 ) ) ) ;
        _lhBeta = ( - 0.015 * ( - _llv - 60.0 ) ) / ( 1.0 - ( exp ( ( - _llv - 60.0 ) / 6.0 ) ) ) ;
        _lhInf = _lhAlpha / ( _lhAlpha + _lhBeta ) ;
        _lhTau = ( 1.0 / ( _lhAlpha + _lhBeta ) ) / _lqt ;

        m = _lmInf;
        h = _lhInf;
    }
}

void mech_state_NaTs2_t(NrnThread *_nt, Mechanism *_ml)
{
    double _v, v;
    int *_ni = _ml->nodeindices;
    int _cntml = _ml->nodecount;
    double * restrict _p = _ml->data;
    int * restrict _ppvar = _ml->pdata;
    double * restrict _vec_v = _nt->_actual_v;
    double * restrict _nt_data = _nt->_data;

    /* insert compiler dependent ivdep like pragma */
    _PRAGMA_FOR_VECTOR_LOOP_
    for (int _iml = 0; _iml < _cntml; ++_iml)
    {
        int _nd_idx = _ni[_iml];
        _v = _vec_v[_nd_idx];
        v=_v;
        ena = _ion_ena;
        double _lmAlpha , _lmBeta , _lmInf , _lmTau , _lhAlpha , _lhBeta , _lhInf , _lhTau , _llv=0.0;
        double _lqt=2.952882641412121 ;

        _llv = v;
        if ( _llv  == - 32.0 )
            _llv = _llv + 0.0001 ;

        _lmAlpha = ( 0.182 * ( _llv - - 32.0 ) ) / ( 1.0 - ( exp ( - ( _llv - - 32.0 ) / 6.0 ) ) ) ;
        _lmBeta = ( 0.124 * ( - _llv - 32.0 ) ) / ( 1.0 - ( exp ( - ( - _llv - 32.0 ) / 6.0 ) ) ) ;
        _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
        _lmTau = ( 1.0 / ( _lmAlpha + _lmBeta ) ) / _lqt ;
        m = m + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / _lmTau)))*(- ( ( ( _lmInf ) ) / _lmTau ) / ( ( ( ( - 1.0) ) ) / _lmTau ) - m) ;

        if ( _llv  == - 60.0 )
          _llv = _llv + 0.0001 ;

        _lhAlpha = ( - 0.015 * ( _llv - - 60.0 ) ) / ( 1.0 - ( exp ( ( _llv - - 60.0 ) / 6.0 ) ) ) ;
        _lhBeta = ( - 0.015 * ( - _llv - 60.0 ) ) / ( 1.0 - ( exp ( ( - _llv - 60.0 ) / 6.0 ) ) ) ;
        _lhInf = _lhAlpha / ( _lhAlpha + _lhBeta ) ;
        _lhTau = ( 1.0 / ( _lhAlpha + _lhBeta ) ) / _lqt ;
        h = h + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / _lhTau)))*(- ( ( ( _lhInf ) ) / _lhTau ) / ( ( ( ( - 1.0) ) ) / _lhTau ) - h) ;
    }
}

void mech_current_NaTs2_t(NrnThread *_nt, Mechanism *_ml)
{
    double* _p = _ml->data;
    int* _ppvar = _ml->pdata;
    int* _ni = _ml->nodeindices;
    int _cntml = _ml->nodecount;
    double * _vec_rhs = _nt->_actual_rhs;
    double * _vec_d = _nt->_actual_d;
    double * _nt_data = _nt->_data;
    double * _vec_v = _nt->_actual_v;

    double _rhs, _g, _v;
    double _lgNaTs2_t , _lina ;
    int _nd_idx;

    /* insert compiler dependent ivdep like pragma */
    _PRAGMA_FOR_VECTOR_LOOP_
    for (int _iml = 0; _iml < _cntml; ++_iml)
    {
        _nd_idx = _ni[_iml];
        _v = _vec_v[_nd_idx];
        ena = _ion_ena;
        _lgNaTs2_t = gNaTs2_tbar * m * m * m * h ;
        _lina = _lgNaTs2_t * ( _v - ena ) ;
        _rhs = _lina;
        _g = _lgNaTs2_t;
        _ion_dinadv += _lgNaTs2_t;
        _ion_ina += _lina ;
     	_vec_rhs[_nd_idx] -= _rhs;
	    _vec_d[_nd_idx] += _g;
    }
}
