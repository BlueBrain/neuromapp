//from Na.c, with big clean up, by cleaning I already tune ....

//#include "Memb_list.h"
#include <cmath>
#include <iostream>
   
 static double A_NMDA0 = 0;
 static double A_AMPA0 = 0;
 static double B_NMDA0 = 0;
 static double B_AMPA0 = 0;
 static double delta_t = 0.01;



//#define t 10  // TIM VALUE
#define dt  0.01 // TIM VALUE 
#define tau_r_AMPA _p[0]
#define tau_d_AMPA _p[1]
#define tau_r_NMDA _p[2]
#define tau_d_NMDA _p[3]
#define Use _p[4]
#define Dep _p[5]
#define Fac _p[6]
#define e _p[7]
#define mg _p[8]
#define u0 _p[9]
#define synapseID _p[10]
#define verboseLevel _p[11]
#define NMDA_ratio _p[12]
#define i _p[13]
#define i_AMPA _p[14]
#define i_NMDA _p[15]
#define g_AMPA _p[16]
#define g_NMDA _p[17]
#define g _p[18]
#define Rstate _p[19]
#define tsyn_fac _p[20]
#define u _p[21]
#define A_AMPA _p[22]
#define B_AMPA _p[23]
#define A_NMDA _p[24]
#define B_NMDA _p[25]
#define factor_AMPA _p[26]
#define factor_NMDA _p[27]
#define DA_AMPA _p[28]
#define DB_AMPA _p[29]
#define DA_NMDA _p[30]
#define DB_NMDA _p[31]
#define v _p[32]
#define _g _p[33]
#define _tsav _p[34]


int states (double* _p){ // I keeep this dummy arg I do not want tune the code for its first release
    A_AMPA = A_AMPA + (1. - exp(dt*(( - 1.0 ) / tau_r_AMPA)))*(- ( 0.0 ) / ( ( - 1.0 ) / tau_r_AMPA ) - A_AMPA) ;
    B_AMPA = B_AMPA + (1. - exp(dt*(( - 1.0 ) / tau_d_AMPA)))*(- ( 0.0 ) / ( ( - 1.0 ) / tau_d_AMPA ) - B_AMPA) ;
    A_NMDA = A_NMDA + (1. - exp(dt*(( - 1.0 ) / tau_r_NMDA)))*(- ( 0.0 ) / ( ( - 1.0 ) / tau_r_NMDA ) - A_NMDA) ;
    B_NMDA = B_NMDA + (1. - exp(dt*(( - 1.0 ) / tau_d_NMDA)))*(- ( 0.0 ) / ( ( - 1.0 ) / tau_d_NMDA ) - B_NMDA) ;
    return 0;
}


void initmodel(double* _p){
        A_NMDA = A_NMDA0;
        A_AMPA = A_AMPA0;
        B_NMDA = B_NMDA0;
        B_AMPA = B_AMPA0;
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
}

double _nrn_current(double* _p, double _v){
    v = _v;
    double _current=0.; // <------------------------------------------------------ value i
    double gmax = 1.0; // <-------------------------- my value
    double mggate = 1.0 / ( 1.0 + exp ( 0.062 * - ( v ) ) * ( mg / 3.57 ) ) ;
    g_AMPA = gmax * ( B_AMPA - A_AMPA ) ;
    g_NMDA = gmax * ( B_NMDA - A_NMDA ) * mggate ;
    g = g_AMPA + g_NMDA ;
    i_AMPA = g_AMPA * ( v - e ) ;
    i_NMDA = g_NMDA * ( v - e ) ;
    i = i_AMPA + i_NMDA ;
    _current += i;
    return _current;
}

void nrn_cur(double *pdata, int num_mechs) {
    double* _p; 
    int* _ni;
    double _rhs, _v(0.); // <--------------------------------------- check value v
    int _iml, _cntml;
    double _nd_area = 3.14 ; // <---------------- my value
    _cntml = num_mechs;

    for (_iml = 0; _iml < _cntml; ++_iml) {
        _p = &pdata[_iml*35];
        _g = _nrn_current(_p, _v + .001);
        _rhs = _nrn_current(_p, _v);
        _g = (_g - _rhs)/.001;
        _g *=  1.e2/(_nd_area);
        _rhs *= 1.e2/(_nd_area);
    }
}

void nrn_init(double *pdata, int num_mechs){
    double* _p; 
 //   double _v; 
    int _iml, _cntml;

    _cntml = num_mechs; // WHAT IS IT ?

    for (_iml = 0; _iml < _cntml; ++_iml) {
        _p = &pdata[_iml*35]; //&_ml->_data[_iml*18]; // I add 18
        initmodel(_p);
    }
}

void nrn_state(double *pdata, int num_mechs) {
    double _break, _save;
    double* _p;
//    double _v;
    int _iml, _cntml;
    double t = 0.0;
    _cntml = num_mechs; // WHAT IS IT ?

    for (_iml = 0; _iml < _cntml; ++_iml) {
        _p = &pdata[_iml*35]; // INIT ?
        _break = t + .5*dt;
        _save = t;
      //  v=_v;
       // ena = _ion_ena;
        for (; t < _break; t = t +  dt) // NOT REPRODUCIBLE AT ALL
            states(_p);
        t = _save;
    }
}

