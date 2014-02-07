#include <cmath>

#define dt 0.01
#define gIhbar _p[0]
#define ihcn _p[1]
#define gIh _p[2]
#define m _p[3]
#define mInf _p[4]
#define mTau _p[5]
#define mAlpha _p[6]
#define mBeta _p[7]
#define Dm _p[8]
#define v _p[9]
#define _g _p[10]

int  rates (double*  _p){
    if ( v  == - 154.9 ) {
        v = v + 0.0001 ;
    }
    mAlpha = 0.001 * 6.43 * ( v + 154.9 ) / ( exp ( ( v + 154.9 ) / 11.9 ) - 1.0 ) ;
    mBeta = 0.001 * 193.0 * exp ( v / 33.1 ) ;
    mInf = mAlpha / ( mAlpha + mBeta ) ;
    mTau = 1.0 / ( mAlpha + mBeta ) ;
    return 0;
}

int states (double* _p){
    rates (_p) ;
    m = m + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / mTau)))*(- ( ( ( mInf ) ) / mTau ) / ( ( ( ( - 1.0) ) ) / mTau ) - m) ;
    return 0;
}
