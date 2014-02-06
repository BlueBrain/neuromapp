/* Created by Language version: 6.2.0 */
/* VECTORIZED */
#include <cmath>
#define dt 0.01
#define gNaTs2_tbar _p[0]
#define ina _p[1]
#define gNaTs2_t _p[2]
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
#define _ion_ena	*_ppvar[0]._pval
#define _ion_ina	*_ppvar[1]._pval
#define _ion_dinadv	*_ppvar[2]._pval
 
int  rates (double * _p){
    double _lqt ;
    _lqt = pow( 2.3 , ( ( 34.0 - 21.0 ) / 10.0 ) ) ;
    if ( v  == - 32.0 ) 
         v = v + 0.0001 ;
    mAlpha = ( 0.182 * ( v - - 32.0 ) ) / ( 1.0 - ( exp ( - ( v - - 32.0 ) / 6.0 ) ) ) ;
    mBeta = ( 0.124 * ( - v - 32.0 ) ) / ( 1.0 - ( exp ( - ( - v - 32.0 ) / 6.0 ) ) ) ;
    mInf = mAlpha / ( mAlpha + mBeta ) ;
    mTau = ( 1.0 / ( mAlpha + mBeta ) ) / _lqt ;
    if ( v  == - 60.0 )
      v = v + 0.0001 ;
    hAlpha = ( - 0.015 * ( v - - 60.0 ) ) / ( 1.0 - ( exp ( ( v - - 60.0 ) / 6.0 ) ) ) ;
    hBeta = ( - 0.015 * ( - v - 60.0 ) ) / ( 1.0 - ( exp ( ( - v - 60.0 ) / 6.0 ) ) ) ;
    hInf = hAlpha / ( hAlpha + hBeta ) ;
    hTau = ( 1.0 / ( hAlpha + hBeta ) ) / _lqt ;
      return 0;
}

int states (double* _p) {
    rates ( _p ) ;
    m = m + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / mTau)))*(- ( ( ( mInf ) ) / mTau ) / ( ( ( ( - 1.0) ) ) / mTau ) - m) ;
    h = h + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / hTau)))*(- ( ( ( hInf ) ) / hTau ) / ( ( ( ( - 1.0) ) ) / hTau ) - h) ;
    return 0;
}
 
