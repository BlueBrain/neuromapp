#include <cmath>

#define dt 0.01
#define gSKv3_1bar _p[0]
#define ik _p[1]
#define gSKv3_1 _p[2]
#define m _p[3]
#define ek _p[4]
#define mInf _p[5]
#define mTau _p[6]
#define Dm _p[7]
#define v _p[8]
#define _g _p[9]
#define _ion_ek	*_ppvar[0]._pval
#define _ion_ik	*_ppvar[1]._pval
#define _ion_dikdv	*_ppvar[2]._pval


 /*END CVODE*/
int  rates (double *  _p){
    mInf = 1.0 / ( 1.0 + exp ( ( ( v - ( 18.700 ) ) / ( - 9.700 ) ) ) ) ;
    mTau = 0.2 * 20.000 / ( 1.0 + exp ( ( ( v - ( - 46.560 ) ) / ( - 44.140 ) ) ) ) ;
    return 0;
 }

int states (double* _p){ // I keeep this dummy arg I do not want tune the code for its first release
    rates ( _p ) ;
    m = m + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / mTau)))*(- ( ( ( mInf ) ) / mTau ) / ( ( ( ( - 1.0) ) ) / mTau ) - m) ;
    return 0;
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
        _p = &pdata[_iml*10]; // INIT ?
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
