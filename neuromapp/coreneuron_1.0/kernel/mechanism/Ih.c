/* Created by Language version: 6.2.0 */
/* VECTORIZED */

#include "coreneuron_1.0/kernel/mechanism/mechanism.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/writer.h"
#include "coreneuron_1.0/common/util/vectorizer.h"

#define _STRIDE _cntml + _iml
#define t _nt->_t

#define gIhbar _p[0*_STRIDE]
#define m _p[1*_STRIDE]
#define gIh _p[2*_STRIDE]
#define Dm _p[3*_STRIDE]
#define _v_unused _p[4*_STRIDE]
#define _g_unused _p[5*_STRIDE]

void mech_init_Ih(NrnThread* _nt, Mechanism *_ml){
    double* _p;
    double _v, v;
    int* _ni;
    int _iml, _cntml;
    _ni = _ml->nodeindices;
    _cntml = _ml->nodecount;
    double * restrict _vec_v = _nt->_actual_v;
    _p = _ml->data;

    _PRAGMA_FOR_VECTOR_LOOP_
    for (_iml = 0; _iml < _cntml; ++_iml)
    {
       int _nd_idx = _ni[_iml];
       _v = _vec_v[_nd_idx];
       v = _v;
       double _lmAlpha , _lmBeta , _lmInf , _llv ;
       _llv = v ;
       if ( _llv  == - 154.9 ) {
          _llv = _llv + 0.0001 ;
          v = _llv ;
       }
       _lmAlpha =  0.001 * 6.43 * ( _llv + 154.9 ) / ( exp ( ( _llv + 154.9 ) / 11.9 ) - 1.0 ) ;
       _lmBeta =  0.001 * 193.0 * exp ( _llv / 33.1 ) ;
       _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
       m = _lmInf ;
    }
}

void mech_current_Ih(NrnThread* _nt, Mechanism* _ml) {
    double* _p;
    int* _ni;
    double _rhs, _g, _v;
    int _iml, _cntml;
    _ni = _ml->nodeindices;
    _cntml = _ml->nodecount;
    double ehcn = -45;
    double * restrict _vec_rhs = _nt->_actual_rhs;
    double * restrict _vec_d = _nt->_actual_d;
    double * restrict _vec_v = _nt->_actual_v;
    _p = _ml->data;


    _PRAGMA_FOR_VECTOR_LOOP_
    for (_iml = 0; _iml < _cntml; ++_iml)
    {
        int _nd_idx = _ni[_iml];
        _v = _vec_v[_nd_idx];
        double _lgIh , _lihcn ;
        _lgIh = gIhbar * m ;
        _lihcn = _lgIh * ( _v - ehcn ) ;
        _rhs = _lihcn;
        _g = _lgIh;
	_vec_rhs[_nd_idx] -= _rhs;
	_vec_d[_nd_idx] += _g;
    }
}

void mech_state_Ih(NrnThread* _nt, Mechanism* _ml) {
    double* _p;
    int* _ppvar;
    double v, _v = 0.0;
    double dt = 0.1;
    int* _ni;
    int _iml, _cntml;
    double * restrict _vec_v = _nt->_actual_v;

    _ni = _ml->nodeindices;
    _cntml = _ml->nodecount;
    _p = _ml->data;
    _ppvar = _ml->pdata;

    _PRAGMA_FOR_VECTOR_LOOP_
    for (_iml = 0; _iml < _cntml; ++_iml)
    {
        double _lmAlpha , _lmBeta , _lmInf , _lmTau , _llv ;
        int _nd_idx = _ni[_iml];
        _v = _vec_v[_nd_idx];
        v=_v;
        _llv = v ;
        if ( _llv  == - 154.9 ) {
          _llv = _llv + 0.0001 ;
          v = _llv ;
          }
        _lmAlpha = 0.001 * 6.43 * ( _llv + 154.9 ) / ( exp ( ( _llv + 154.9 ) / 11.9 ) - 1.0 ) ;
        _lmBeta =   0.001 * 193.0 * exp ( _llv / 33.1 ) ;
        _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
        _lmTau = 1.0 / ( _lmAlpha + _lmBeta ) ;
         m = m + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / _lmTau)))*(- ( ( ( _lmInf ) ) / _lmTau ) / ( ( ( ( - 1.0) ) ) / _lmTau ) - m) ;
    }
}
