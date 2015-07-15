/* Created by Language version: 6.2.0 */
/* VECTORIZED */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "coreneuron/mech/cfile/scoplib.h"
#undef PI
 
#include "coreneuron/nrnoc/md1redef.h"
#include "coreneuron/nrnconf.h"
#include "coreneuron/nrnoc/multicore.h"

#include "coreneuron/utils/randoms/nrnran123.h"

#include "coreneuron/nrnoc/md2redef.h"
#if METHOD3
extern int _method3;
#endif

#if !NRNGPU
#if !defined(DISABLE_HOC_EXP)
#undef exp
#define exp hoc_Exp
#endif
extern double hoc_Exp(double);
#endif
 
#if defined(__clang__)
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("clang loop vectorize(enable)")
#elif defined(__ICC) || defined(__INTEL_COMPILER)
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("ivdep")
#elif defined(__GNUC__) || defined(__GNUG__)
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("GCC ivdep")
#elif defined(__IBMC__) || defined(__IBMCPP__)
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("ibm independent_loop")
#elif defined(__PGI)
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("vector")
#elif defined(_CRAYC)
#define _PRAGMA_FOR_VECTOR_LOOP_ _Pragma("_CRI ivdep")
#else
#define _PRAGMA_FOR_VECTOR_LOOP_
#endif // _PRAGMA_FOR_VECTOR_LOOP_
 
#if !defined(LAYOUT)
/* 1 means AoS, >1 means AoSoA, <= 0 means SOA */
#define LAYOUT 1
#endif
#if LAYOUT >= 1
#define _STRIDE LAYOUT
#else
#define _STRIDE _cntml + _iml
#endif
 
#define nrn_init _nrn_init__Ih
#define nrn_cur _nrn_cur__Ih
#define _nrn_current _nrn_current__Ih
#define nrn_jacob _nrn_jacob__Ih
#define nrn_state _nrn_state__Ih
#define _net_receive _net_receive__Ih 
#define states states__Ih 
 
#define _threadargscomma_ _iml, _cntml, _p, _ppvar, _thread, _nt, v,
#define _threadargsprotocomma_ int _iml, int _cntml, double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt, double v,
#define _threadargs_ _iml, _cntml, _p, _ppvar, _thread, _nt, v
#define _threadargsproto_ int _iml, int _cntml, double* _p, Datum* _ppvar, ThreadDatum* _thread, _NrnThread* _nt, double v
 	/*SUPPRESS 761*/
	/*SUPPRESS 762*/
	/*SUPPRESS 763*/
	/*SUPPRESS 765*/
	 extern double *getarg();
 /* Thread safe. No static _p or _ppvar. */
 
#define t _nt->_t
#define dt _nt->_dt
#define gIhbar _p[0*_STRIDE]
#define m _p[1*_STRIDE]
#define gIh _p[2*_STRIDE]
#define Dm _p[3*_STRIDE]
#define _v_unused _p[4*_STRIDE]
#define _g_unused _p[5*_STRIDE]
 
#if MAC
#if !defined(v)
#define v _mlhv
#endif
#if !defined(h)
#define h _mlhh
#endif
#endif
 
#if defined(__cplusplus)
extern "C" {
#endif
 static int hoc_nrnpointerindex =  -1;
 static ThreadDatum* _extcall_thread;
 /* external NEURON variables */
 
#if 0 /*BBCORE*/
 /* declaration of user functions */
 static void _hoc_mBetaf(void);
 static void _hoc_mAlphaf(void);
 
#endif /*BBCORE*/
 static int _mechtype;
extern int nrn_get_mechtype();
extern void hoc_register_prop_size(int, int, int);
extern Memb_func* memb_func;
 
#if 0 /*BBCORE*/
 /* connect user functions to hoc names */
 static VoidFunc hoc_intfunc[] = {
 "setdata_Ih", _hoc_setdata,
 "mBetaf_Ih", _hoc_mBetaf,
 "mAlphaf_Ih", _hoc_mAlphaf,
 0, 0
};
 
#endif /*BBCORE*/
#define mBetaf mBetaf_Ih
#define mAlphaf mAlphaf_Ih
 inline double mBetaf( _threadargsprotocomma_ double );
 inline double mAlphaf( _threadargsprotocomma_ double );
 /* declare global and static user variables */
#define ehcn ehcn_Ih
 double ehcn = -45;
 
#if 0 /*BBCORE*/
 /* some parameters have upper and lower limits */
 static HocParmLimits _hoc_parm_limits[] = {
 0,0,0
};
 static HocParmUnits _hoc_parm_units[] = {
 "ehcn_Ih", "mV",
 "gIhbar_Ih", "S/cm2",
 0,0
};
 
#endif /*BBCORE*/
 static double delta_t = 0.01;
 static double m0 = 0;
 
#if 0 /*BBCORE*/
 /* connect global user variables to hoc */
 static DoubScal hoc_scdoub[] = {
 "ehcn_Ih", &ehcn_Ih,
 0,0
};
 static DoubVec hoc_vdoub[] = {
 0,0,0
};
 
#endif /*BBCORE*/
 static double _sav_indep;
 static void nrn_alloc(double*, Datum*, int);
static void  nrn_init(_NrnThread*, _Memb_list*, int);
static void nrn_state(_NrnThread*, _Memb_list*, int);
 static void nrn_cur(_NrnThread*, _Memb_list*, int);
 /* connect range variables in _p that hoc is supposed to know about */
 static const char *_mechanism[] = {
 "6.2.0",
"Ih",
 "gIhbar_Ih",
 0,
 0,
 "m_Ih",
 0,
 0};
 
static void nrn_alloc(double* _p, Datum* _ppvar, int _type) {
 
#if 0 /*BBCORE*/
 	/*initialize range parameters*/
 	gIhbar = 1e-05;
 
#endif /* BBCORE */
 
}
 static void _initlists();
 
#define _psize 6
#define _ppsize 0
 extern Symbol* hoc_lookup(const char*);
extern void _nrn_thread_reg(int, int, void(*f)(Datum*));
extern void _nrn_thread_table_reg(int, void(*)(_threadargsproto_, int));
extern void _cvode_abstol( Symbol**, double*, int);

 void _Ih_reg() {
	int _vectorized = 1;
  _initlists();
 _mechtype = nrn_get_mechtype(_mechanism[1]);
 if (_mechtype == -1) return;
 _nrn_layout_reg(_mechtype, LAYOUT);
 
#if 0 /*BBCORE*/
 
#endif /*BBCORE*/
 	register_mech(_mechanism, nrn_alloc,nrn_cur, NULL, nrn_state, nrn_init, hoc_nrnpointerindex, 1);
  hoc_register_prop_size(_mechtype, _psize, _ppsize);
 }
static char *modelname = "";

static int error;
static int _ninits = 0;
static int _match_recurse=1;
static void _modl_cleanup(){ _match_recurse=1;}
 
static int _ode_spec1(_threadargsproto_);
static int _ode_matsol1(_threadargsproto_);
 static int _slist1[1], _dlist1[1];
 static int states(_threadargsproto_);
 
/*CVODE*/
 static int _ode_spec1 (_threadargsproto_) {int _reset = 0; {
   double _lmAlpha , _lmBeta , _lmInf , _lmTau , _llv ;
 _llv = v ;
   if ( _llv  == - 154.9 ) {
     _llv = _llv + 0.0001 ;
     v = _llv ;
     }
   _lmAlpha = mAlphaf ( _threadargscomma_ _llv ) ;
   _lmBeta = mBetaf ( _threadargscomma_ _llv ) ;
   _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
   _lmTau = 1.0 / ( _lmAlpha + _lmBeta ) ;
   Dm = ( _lmInf - m ) / _lmTau ;
   }
 return _reset;
}
 static int _ode_matsol1 (_threadargsproto_) {
 double _lmAlpha , _lmBeta , _lmInf , _lmTau , _llv ;
 _llv = v ;
 if ( _llv  == - 154.9 ) {
   _llv = _llv + 0.0001 ;
   v = _llv ;
   }
 _lmAlpha = mAlphaf ( _threadargscomma_ _llv ) ;
 _lmBeta = mBetaf ( _threadargscomma_ _llv ) ;
 _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
 _lmTau = 1.0 / ( _lmAlpha + _lmBeta ) ;
 Dm = Dm  / (1. - dt*( ( ( ( - 1.0 ) ) ) / _lmTau )) ;
 return 0;
}
 /*END CVODE*/
 static int states (_threadargsproto_) { {
   double _lmAlpha , _lmBeta , _lmInf , _lmTau , _llv ;
 _llv = v ;
   if ( _llv  == - 154.9 ) {
     _llv = _llv + 0.0001 ;
     v = _llv ;
     }
   _lmAlpha = mAlphaf ( _threadargscomma_ _llv ) ;
   _lmBeta = mBetaf ( _threadargscomma_ _llv ) ;
   _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
   _lmTau = 1.0 / ( _lmAlpha + _lmBeta ) ;
    m = m + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / _lmTau)))*(- ( ( ( _lmInf ) ) / _lmTau ) / ( ( ( ( - 1.0) ) ) / _lmTau ) - m) ;
   }
  return 0;
}
 
double mAlphaf ( _threadargsprotocomma_ double _lv ) {
   double _lmAlphaf;
 _lmAlphaf = 0.001 * 6.43 * ( _lv + 154.9 ) / ( exp ( ( _lv + 154.9 ) / 11.9 ) - 1.0 ) ;
   
return _lmAlphaf;
 }
 
#if 0 /*BBCORE*/
 
static void _hoc_mAlphaf(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 _r =  mAlphaf ( _threadargs_, *getarg(1) ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/
 
double mBetaf ( _threadargsprotocomma_ double _lv ) {
   double _lmBetaf;
 _lmBetaf = 0.001 * 193.0 * exp ( _lv / 33.1 ) ;
   
return _lmBetaf;
 }
 
#if 0 /*BBCORE*/
 
static void _hoc_mBetaf(void) {
  double _r;
   double* _p; Datum* _ppvar; ThreadDatum* _thread; _NrnThread* _nt;
   if (_extcall_prop) {_p = _extcall_prop->param; _ppvar = _extcall_prop->dparam;}else{ _p = (double*)0; _ppvar = (Datum*)0; }
  _thread = _extcall_thread;
  _nt = nrn_threads;
 _r =  mBetaf ( _threadargs_, *getarg(1) ;
 hoc_retpushx(_r);
}
 
#endif /*BBCORE*/

static void initmodel(_threadargsproto_) {
  int _i; double _save;{
  m = m0;
 {
   double _lmAlpha , _lmBeta , _lmInf , _llv ;
 _llv = v ;
   if ( _llv  == - 154.9 ) {
     _llv = _llv + 0.0001 ;
     v = _llv ;
     }
   _lmAlpha = mAlphaf ( _threadargscomma_ _llv ) ;
   _lmBeta = mBetaf ( _threadargscomma_ _llv ) ;
   _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
   m = _lmInf ;
   }
 
}
}

static void nrn_init(_NrnThread* _nt, _Memb_list* _ml, int _type){
double* _p; Datum* _ppvar; ThreadDatum* _thread;
double _v, v; int* _ni; int _iml, _cntml;
    _ni = _ml->_nodeindices;
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
double * _nt_data = _nt->_data;
double * _vec_v = _nt->_actual_v;
#if LAYOUT == 1 /*AoS*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
#endif
#if LAYOUT == 0 /*SoA*/
 _p = _ml->_data; _ppvar = _ml->_pdata;
for (_iml = 0; _iml < _cntml; ++_iml) {
#endif
#if LAYOUT > 1 /*AoSoA*/
#error AoSoA not implemented.
#endif
    int _nd_idx = _ni[_iml];
    _v = _vec_v[_nd_idx];
 v = _v;
 initmodel(_threadargs_);
}
}

static void nrn_cur(_NrnThread* _nt, _Memb_list* _ml, int _type) {
double* _p; Datum* _ppvar; ThreadDatum* _thread;
int* _ni; double _rhs, _g, _v, v; int _iml, _cntml;
    _ni = _ml->_nodeindices;
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
double * _vec_rhs = _nt->_actual_rhs;
double * _vec_d = _nt->_actual_d;
double * _nt_data = _nt->_data;
double * _vec_v = _nt->_actual_v;
#if LAYOUT == 1 /*AoS*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
#endif
#if LAYOUT == 0 /*SoA*/
 _p = _ml->_data; _ppvar = _ml->_pdata;
/* insert compiler dependent ivdep like pragma */
_PRAGMA_FOR_VECTOR_LOOP_
for (_iml = 0; _iml < _cntml; ++_iml) {
#endif
#if LAYOUT > 1 /*AoSoA*/
#error AoSoA not implemented.
#endif
    int _nd_idx = _ni[_iml];
    _v = _vec_v[_nd_idx];
 {
 double _lgIh , _lihcn ;
 _lgIh = gIhbar * m ;
 _lihcn = _lgIh * ( _v - ehcn ) ;
  _rhs = _lihcn;
  _g = _lgIh;
 }
	_vec_rhs[_nd_idx] -= _rhs;
	_vec_d[_nd_idx] += _g;
 
}
 
}

static void nrn_state(_NrnThread* _nt, _Memb_list* _ml, int _type) {
double* _p; Datum* _ppvar; ThreadDatum* _thread;
double v, _v = 0.0; int* _ni; int _iml, _cntml;
    _ni = _ml->_nodeindices;
_cntml = _ml->_nodecount;
_thread = _ml->_thread;
double * _nt_data = _nt->_data;
double * _vec_v = _nt->_actual_v;
#if LAYOUT == 1 /*AoS*/
for (_iml = 0; _iml < _cntml; ++_iml) {
 _p = _ml->_data + _iml*_psize; _ppvar = _ml->_pdata + _iml*_ppsize;
#endif
#if LAYOUT == 0 /*SoA*/
 _p = _ml->_data; _ppvar = _ml->_pdata;
/* insert compiler dependent ivdep like pragma */
_PRAGMA_FOR_VECTOR_LOOP_
for (_iml = 0; _iml < _cntml; ++_iml) {
#endif
#if LAYOUT > 1 /*AoSoA*/
#error AoSoA not implemented.
#endif
    int _nd_idx = _ni[_iml];
    _v = _vec_v[_nd_idx];
 v=_v;
{
 {   states(_threadargs_);
  }}}

}

static void terminal(){}

static void _initlists(){
 double _x; double* _p = &_x;
 int _i; static int _first = 1;
 int _cntml=0;
 int _iml=0;
  if (!_first) return;
 _slist1[0] = &(m) - _p;  _dlist1[0] = &(Dm) - _p;
_first = 0;
}

#if defined(__cplusplus)
} /* extern "C" */
#endif

