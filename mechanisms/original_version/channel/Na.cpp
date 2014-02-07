//from Na.c, with big clean up, by cleaning I already tune ....
#include <cmath>
#include <iostream>

static double h0 = 0;
static double m0 = 0;

//#define t 10  // TIM VALUE
#define dt  0.01 // TIM VALUE
#define gNabar _p[0]
#define ina _p[1]
#define gNa _p[2]
#define m _p[3]
#define h _p[4]
#define ena _p[5]
#define mInf _p[6]
#define mTau _p[7]
#define mAlpha _p[8]
#define mBeta _p[9]
#define hInf _p[10]
#define hTau _p[11]
#define hAlpha _p[12]
#define hBeta _p[13]
#define Dm _p[14]
#define Dh _p[15]
#define v _p[16]
#define _g _p[17]

//#define _ion_ena	*_ppvar[0]._pval // which value, if I give a value a tune moving from dynamic to static approach
//#define _ion_ina	*_ppvar[1]._pval // which value
//#define _ion_dinadv	*_ppvar[2]._pval // which value
int  rates( double *_p){
    if ( v  == - 35.0 ) {
        v = v + 0.0001 ;
    }

    mAlpha = ( 0.182 * ( v - - 35.0 ) )/ ( 1.0 - ( exp ( - ( v - - 35.0 ) / 9.0 ) ) ) ;
    mBeta = ( 0.124 * ( - v - 35.0 ) )  / ( 1.0 - ( exp ( - ( - v - 35.0 ) / 9.0 ) ) ) ;
    mInf = mAlpha / ( mAlpha + mBeta ) ;
    mTau = 1.0 / ( mAlpha + mBeta ) ;

    if ( v  == - 50.0 ) {
         v = v + 0.0001 ;
    }

    hAlpha = ( 0.024 * ( v - - 50.0 ) ) / ( 1.0 - ( exp ( - ( v - - 50.0 ) / 5.0 ) ) ) ;

    if ( v  == - 75.0 ) {
         v = v + 0.0001 ;
    }

    hBeta = ( 0.0091 * ( - v - 75.0 ) ) / ( 1.0 - ( exp ( - ( - v - 75.0 ) / 5.0 ) ) ) ;
    hInf = 1.0 / ( 1.0 + exp ( ( v - - 65.0 ) / 6.2 ) ) ;
    hTau = 1.0 / ( hAlpha + hBeta ) ;

    return 0;
}

int states (double* _p){ // I keeep this dummy arg I do not want tune the code for its first release
    rates(_p);
    m = m + (1. - exp(dt*(( ( ( -1.0 ) ) ) / mTau)))*(- ( ( ( mInf ) ) / mTau ) / ( ( ( ( -1.0) ) ) / mTau ) - m);
    h = h + (1. - exp(dt*(( ( ( -1.0 ) ) ) / hTau)))*(- ( ( ( hInf ) ) / hTau ) / ( ( ( ( -1.0) ) ) / hTau ) - h);
    return 0;
}


void initmodel(double* _p){
  h = h0; // 0
  m = m0; // 0
  rates (_p);
  m = mInf;
  h = hInf;
}

double _nrn_current(double* _p, double _v){
   double _current=0.;
   v=_v;
   gNa = gNabar * m * m * m * h ;
   ina = gNa * ( v - ena ) ;
   _current += ina;
   return _current;
}

void nrn_cur(double *pdata, int num_mechs) {
    double* _p;
  //  Node *_nd;
//    int* _ni;
    double _rhs, _v(-33.33); // _v voltage fron a node random number
    int _iml, _cntml;
    _cntml = num_mechs;
    double _ion_dinadv(0.0);
    double _ion_ina(0.0);
    for (_iml = 0; _iml < _cntml; ++_iml) {
        _p = &pdata[_iml*18];
     // _ppvar = _ml->_pdata[_iml];
     //   _nd = &_ml->_nodelist[_iml]; // WHAT DO I DO ?
     //   _v = NODEV(_nd); // WHAT DO I DO ?
     //   ena = _ion_ena;
        _g = _nrn_current(_p, _v + .001);
        double _dina;
        _dina = ina;
        _rhs = _nrn_current(_p, _v);
        _ion_dinadv += (_dina - ina)/.001 ;
        _g = (_g - _rhs)/.001;
        _ion_ina += ina ;
     //	NODERHS(_nd) -= _rhs; // WHAT IS IT ?
    }
}

void nrn_init(double *pdata, int num_mechs){
    double* _p; 
 //   double _v; 
    int _iml, _cntml;

    _cntml = num_mechs; // e.g. 1024 channel type Na

    for (_iml = 0; _iml < _cntml; ++_iml) {
        _p = &pdata[_iml*18]; //&_ml->_data[_iml*18]; // I add 18
   //     _ppvar = _ml->_pdata[_iml];
   //     _nd = _ml->_nodelist[_iml];
   //     _v = NODEV(_nd); // WHAT IS IT ?
   //     v = _v;
   //     ena = _ion_ena;
        initmodel(_p);
    }
}

void nrn_state(double *pdata, int num_mechs) {
    double _break, _save;
    double* _p;
//    double _v;
    int _iml, _cntml;
    double t = 0.0;// <------------------ my value, which value ?
    _cntml = num_mechs; // WHAT IS IT ?

    //make sure set OMP_SCHEDULE environmental varible
    #pragma omp parallel for default(none) shared(pdata, num_mechs, _cntml) private(_iml, _p, _break, _save, t) schedule(runtime)
    for (_iml = 0; _iml < _cntml; ++_iml) {
        _p = &pdata[_iml*18]; // INIT ?
        _break = t + .5*dt;
        _save = t;
      //  v=_v;
       // ena = _ion_ena;
//           for(int i=0; i < 5; ++i)
//              states(_p);

        for (; t < _break; t +=  dt) // NOT REPRODUCIBLE AT ALL
            states(_p);
        t = _save;
    }
}

