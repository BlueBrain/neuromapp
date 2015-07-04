

#ifdef __CUDACC__
#include <stdio.h>
#define PREFIX __device__
#define NA_STATE Na_state
#define NA_CURRENT Na_current

// Implementation extracted from CUDA Programming Guide
__device__ double atomicAdd(double* address, double val)
{
    unsigned long long int* address_as_ull =
                              (unsigned long long int*)address;
    unsigned long long int old = *address_as_ull, assumed;

    do {
        assumed = old;
        old = atomicCAS(address_as_ull, assumed,
                        __double_as_longlong(val +
                               __longlong_as_double(assumed)));

    // Note: uses integer comparison to avoid hang in case of NaN (since NaN != NaN)
    } while (assumed != old);

    return __longlong_as_double(old);
}


#else
#define PREFIX 
#define NA_STATE Na_state_cpu
#define NA_CURRENT Na_current_cpu
#endif

PREFIX void NA_STATE(double* __restrict p_0, double* __restrict p_1, double* __restrict p_2, double* __restrict p_3,
                double* __restrict pion_data0, int * __restrict ppvar0, double* __restrict pVEC_V, int idx)
{
        double dt = 0.1;
        double _lmAlpha , _lmBeta , _lmInf , _lmTau , _lhAlpha , _lhBeta , _lhInf , _lhTau , _llv , _lqt, _v ;

        _lqt = 2.952882641412121 ;

        _v = pVEC_V[idx];
        _llv = _v;
        p_3[idx] = pion_data0[ppvar0[idx]];
        if ( _llv == - 32.0 ) {
            _llv = _llv + 0.0001 ;
        }
        _lmAlpha = ( 0.182 * ( _llv - - 32.0 ) ) / ( 1.0 - ( exp ( - ( _llv - - 32.0 ) / 6.0 ) ) );
        _lmBeta = ( 0.124 * ( - _llv - 32.0 ) ) / ( 1.0 - ( exp ( - ( - _llv - 32.0 ) / 6.0 ) ) );
        _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
        _lmTau = ( 1.0 / ( _lmAlpha + _lmBeta ) ) / _lqt ;
        p_0[idx] = p_0[idx] + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / _lmTau)))*(- ( ( ( _lmInf ) ) / _lmTau ) / ( ( ( ( - 1.0) ) ) / _lmTau ) - p_0[idx]) ;
        if ( _llv == - 60.0 ) {
            _llv = _llv + 0.0001 ;
        }
        _lhAlpha = ( - 0.015 * ( _llv - - 60.0 ) ) / ( 1.0 - ( exp ( ( _llv - - 60.0 ) / 6.0 ) ) );
        _lhBeta = ( - 0.015 * ( - _llv - 60.0 ) ) / ( 1.0 - ( exp ( ( - _llv - 60.0 ) / 6.0 ) ) );
        _lhInf = _lhAlpha / ( _lhAlpha + _lhBeta ) ;
        _lhTau = ( 1.0 / ( _lhAlpha + _lhBeta ) ) / _lqt ;
        p_2[idx] = p_2[idx] + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / _lhTau)))*(- ( ( ( _lhInf ) ) / _lhTau ) / ( ( ( ( - 1.0) ) ) / _lhTau ) - p_2[idx]) ;
        _v = _llv ;

}



//#include <stdio.h>
PREFIX void NA_CURRENT(double* __restrict p_0, double* __restrict p_1, double* __restrict p_2, double* __restrict p_3,
                int * __restrict ppvar0, int * __restrict ppvar1, int * __restrict ppvar2, double* __restrict pion_data0,
                double* __restrict pion_data1, double* __restrict pion_data2, double* __restrict pVEC_V,
                double* __restrict pVEC_RHS, int * __restrict pni, int idx)
{
        double _rhs, _g, _v;

        _v = pVEC_V[pni[idx]];
        p_3[idx] = pion_data0[ppvar0[idx]];
        {
            double _lgNaTs2_t , _lina ;
            _lgNaTs2_t = p_0[idx] * p_1[idx] * p_1[idx] * p_1[idx] * p_2[idx];
            _lina = _lgNaTs2_t * ( _v - p_3[idx]) ;
            _rhs = _lina;
            _g = _lgNaTs2_t;


#ifdef __CUDACC__
char a = 'g';
#else
char a = 'c';
#endif


#ifdef __CUDACC__
            double old = atomicAdd(&pion_data2[ppvar2[idx]], _lgNaTs2_t);
            atomicAdd(&pion_data1[ppvar1[idx]], _lina);
#else
            double old = pion_data2[ppvar2[idx]];
            pion_data2[ppvar2[idx]] += _lgNaTs2_t;
            pion_data1[ppvar1[idx]] += _lina ;
#endif

//if (idx > 9)
//printf("[%c][%d] going to add %.64g + %.64g result is %.64g  === %.64g\n", a, idx, old, _lgNaTs2_t, old + _lgNaTs2_t, pion_data2[ppvar2[idx]]);

        }
        pVEC_RHS[pni[idx]] -= _rhs;
        pVEC_V[pni[idx]] += _g;
}

