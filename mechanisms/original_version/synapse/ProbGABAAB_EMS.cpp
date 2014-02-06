#include <cmath>
 
static double A_GABAB0 = 0;
static double A_GABAA0 = 0;
static double B_GABAB0 = 0;
static double B_GABAA0 = 0;
static double delta_t = 0.01;
 
#define dt  0.01 // TIM VALUE 
#define tau_r_GABAA _p[0]
#define tau_d_GABAA _p[1]
#define tau_r_GABAB _p[2]
#define tau_d_GABAB _p[3]
#define Use _p[4]
#define Dep _p[5]
#define Fac _p[6]
#define e_GABAA _p[7]
#define e_GABAB _p[8]
#define u0 _p[9]
#define synapseID _p[10]
#define verboseLevel _p[11]
#define GABAB_ratio _p[12]
#define i _p[13]
#define i_GABAA _p[14]
#define i_GABAB _p[15]
#define g_GABAA _p[16]
#define g_GABAB _p[17]
#define A_GABAA_step _p[18]
#define B_GABAA_step _p[19]
#define A_GABAB_step _p[20]
#define B_GABAB_step _p[21]
#define g _p[22]
#define Rstate _p[23]
#define tsyn_fac _p[24]
#define u _p[25]
#define A_GABAA _p[26]
#define B_GABAA _p[27]
#define A_GABAB _p[28]
#define B_GABAB _p[29]
#define factor_GABAA _p[30]
#define factor_GABAB _p[31]
#define DA_GABAA _p[32]
#define DB_GABAA _p[33]
#define DA_GABAB _p[34]
#define DB_GABAB _p[35]
#define v _p[36]
#define _g _p[37]
#define _tsav _p[38]
#define rng	*_ppvar[2]._pval
#define _p_rng	_ppvar[2]._pval
 
#define gmax 0.001 

double nrn_random_pick(void* r);
void* nrn_random_arg(int argpos);

int  state ( double* _p){
    A_GABAA = A_GABAA * A_GABAA_step ;
    B_GABAA = B_GABAA * B_GABAA_step ;
    A_GABAB = A_GABAB * A_GABAB_step ;
    B_GABAB = B_GABAB * B_GABAB_step ;
    return 0;
}

void initmodel(double* _p) {
    A_GABAB = A_GABAB0;
    A_GABAA = A_GABAA0;
    B_GABAB = B_GABAB0;
    B_GABAA = B_GABAA0;
    double _ltp_GABAA , _ltp_GABAB ;
    Rstate = 1.0 ;
    tsyn_fac = 0.0 ;
    u = u0 ;
    A_GABAA = 0.0 ;
    B_GABAA = 0.0 ;
    A_GABAB = 0.0 ;
    B_GABAB = 0.0 ;
    _ltp_GABAA = ( tau_r_GABAA * tau_d_GABAA ) / ( tau_d_GABAA - tau_r_GABAA ) * log ( tau_d_GABAA / tau_r_GABAA ) ;
    _ltp_GABAB = ( tau_r_GABAB * tau_d_GABAB ) / ( tau_d_GABAB - tau_r_GABAB ) * log ( tau_d_GABAB / tau_r_GABAB ) ;
    factor_GABAA = - exp ( - _ltp_GABAA / tau_r_GABAA ) + exp ( - _ltp_GABAA / tau_d_GABAA ) ;
    factor_GABAA = 1.0 / factor_GABAA ;
    factor_GABAB = - exp ( - _ltp_GABAB / tau_r_GABAB ) + exp ( - _ltp_GABAB / tau_d_GABAB ) ;
    factor_GABAB = 1.0 / factor_GABAB ;
    A_GABAA_step = exp ( dt * ( ( - 1.0 ) / tau_r_GABAA ) ) ;
    B_GABAA_step = exp ( dt * ( ( - 1.0 ) / tau_d_GABAA ) ) ;
    A_GABAB_step = exp ( dt * ( ( - 1.0 ) / tau_r_GABAB ) ) ;
    B_GABAB_step = exp ( dt * ( ( - 1.0 ) / tau_d_GABAB ) ) ;
}

static double _nrn_current(double* _p, double _v){
   double _current=0.;
   v=_v;
   g_GABAA = gmax * ( B_GABAA - A_GABAA ) ;
   g_GABAB = gmax * ( B_GABAB - A_GABAB ) ;
   g = g_GABAA + g_GABAB ;
   i_GABAA = g_GABAA * ( v - e_GABAA ) ;
   i_GABAB = g_GABAB * ( v - e_GABAB ) ;
   i = i_GABAA + i_GABAB ;
   _current += i;
   return _current;
}

void nrn_cur(double *pdata, int num_mechs) {
    int* _ni;
    double _rhs;
    double _v(0.);
    double _nd_area(3.14);
    int _iml, _cntml;
    _cntml = num_mechs;
    double *_p;
    for (_iml = 0; _iml < _cntml; ++_iml) {
        _p = &pdata[_iml*39];
        _g = _nrn_current(_p, _v + .001);
        _rhs = _nrn_current(_p,_v);
        _g = (_g - _rhs)/.001;
        _g *=  1.e2/(_nd_area);
        _rhs *= 1.e2/(_nd_area);
    }
}

void nrn_init(double *pdata, int num_mechs) {
    double* _p;
    int _iml, _cntml;
    _cntml = num_mechs;
    for (_iml = 0; _iml < _cntml; ++_iml) {
         _p = &pdata[_iml*39];
         _tsav = -1e20;
         initmodel(_p);
    }
}

void nrn_state( double* pdata, int num_mechs ) {
 double _break, _save;
double* _p; double t(0);
   double _v; int* _ni; int _iml, _cntml;
    _cntml = num_mechs;
    for (_iml = 0; _iml < _cntml; ++_iml) {
        _p = &pdata[_iml*39];
        _break = t + .5*dt; _save = t;
        for (; t < _break; t += dt) 
            state(_p); 
        t = _save;
     }
}
