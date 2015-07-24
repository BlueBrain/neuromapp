#include "coreneuron_1.0/kernel/mechanism/mechanism.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/writer.h"
#include "coreneuron_1.0/common/util/vectorizer.h"

#define _STRIDE _cntml + _iml
#define t _nt->_t
//#define dt _nt->_dt
#define dt 0.001

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

void mech_init_ProbAMPANMDA_EMS(NrnThread *_nt, Mechanism *_ml)
{
    double v=0.0, _v = 0.0;
    int* _ni = _ml->nodeindices;
    int _cntml = _ml->nodecount;
    double * restrict _vec_v = _nt->_actual_v;
    double * restrict _p = _ml->data;

    _PRAGMA_FOR_VECTOR_LOOP_
    for (int _iml = 0; _iml < _cntml; ++_iml)
    {
        int _nd_idx = _ni[_iml];
        _tsav = -1e20;
        _v = _vec_v[_nd_idx];
        v = _v;
        A_NMDA = 0.;
        A_AMPA = 0.;
        B_NMDA = 0.;
        B_AMPA = 0.;
        double _ltp_AMPA , _ltp_NMDA ;
        Rstate = 1.0 ;
        tsyn_fac = 0.0 ;
        u = u0 ;
        A_AMPA = 0.0 ;
        B_AMPA = 0.0 ;
        A_NMDA = 0.0 ;
        B_NMDA = 0.0 ;
        _ltp_AMPA = ( tau_r_AMPA * tau_d_AMPA ) / ( tau_d_AMPA - tau_r_AMPA ) * log ( tau_d_AMPA / tau_r_AMPA ) ;
        _ltp_NMDA = ( tau_r_NMDA * tau_d_NMDA ) / ( tau_d_NMDA - tau_r_NMDA ) * log ( tau_d_NMDA / tau_r_NMDA ) ;
        factor_AMPA = - exp ( - _ltp_AMPA / tau_r_AMPA ) + exp ( - _ltp_AMPA / tau_d_AMPA ) ;
        factor_AMPA = 1.0 / factor_AMPA ;
        factor_NMDA = - exp ( - _ltp_NMDA / tau_r_NMDA ) + exp ( - _ltp_NMDA / tau_d_NMDA ) ;
        factor_NMDA = 1.0 / factor_NMDA ;
        A_AMPA_step = exp ( dt * ( ( - 1.0 ) / tau_r_AMPA ) ) ;
        B_AMPA_step = exp ( dt * ( ( - 1.0 ) / tau_d_AMPA ) ) ;
        A_NMDA_step = exp ( dt * ( ( - 1.0 ) / tau_r_NMDA ) ) ;
        B_NMDA_step = exp ( dt * ( ( - 1.0 ) / tau_d_NMDA ) ) ;
    }

}

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
