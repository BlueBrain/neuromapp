#include <cmath>

#define dt 0.01
#define gImbar _p[0]
#define ik _p[1]
#define gIm _p[2]
#define m _p[3]
#define ek _p[4]
#define mInf _p[5]
#define mTau _p[6]
#define mAlpha _p[7]
#define mBeta _p[8]
#define Dm _p[9]
#define v _p[10]
#define _g _p[11]
#define _ion_ek	*_ppvar[0]._pval
#define _ion_ik	*_ppvar[1]._pval
#define _ion_dikdv	*_ppvar[2]._pval

int  rates (double* _p){
    double _lqt ;
    _lqt = pow( 2.3 , ( ( 34.0 - 21.0 ) / 10.0 ) ) ;
    mAlpha = 3.3e-3 * exp ( 2.5 * 0.04 * ( v - - 35.0 ) ) ;
    mBeta = 3.3e-3 * exp ( - 2.5 * 0.04 * ( v - - 35.0 ) ) ;
    mInf = mAlpha / ( mAlpha + mBeta ) ;
    mTau = ( 1.0 / ( mAlpha + mBeta ) ) / _lqt ;
    return 0;
}

int  states (double * _p){
    rates ( _p ) ;
    m = m + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / mTau)))*(- ( ( ( mInf ) ) / mTau ) / ( ( ( ( - 1.0) ) ) / mTau ) - m) ;
    return 0;
}

