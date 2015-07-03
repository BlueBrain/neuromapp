#pragma once

static double  ehcn_Ih = 3.241; // global random value

class lh : public mechanism{
public:
    static const int width = 31;
    static std::string name(){return "lh";};
    lh(size_t s, size_t l): mechanism(s,l){}

    void state(){
        double* __restrict p_1 =  (double *)( &c.front()+1*chunk());

        double* __restrict pVEC_V =  (double *)( &VEC_V.front());

        size_t size =  number_instance();
        double _lmAlpha , _lmBeta , _lmInf , _lmTau , _llv, _v ;
        double gmax_ProbAMPANMDA_EMS(1.1); // random value

        int* __restrict pni = (int *)(&ni.front());

        double dt = 0.1;

        //PRAGMA_FOR_VECTOR_LOOP
        #pragma acc kernels
        for (int i = 0; i < size; ++i) {

            _v = pVEC_V[pni[i]];
            _llv = _v ;

            #ifdef __CORENEURON_ORIGINAL
            if ( _llv == - 154.9 ) {
                _llv = _llv + 0.0001 ;
            }
            #endif

            _lmAlpha = 0.001 * 6.43 * ( _llv + 154.9 ) / ( exp ( ( _llv + 154.9 ) / 11.9 ) - 1.0 );
            _lmBeta = 0.001 * 193.0 * exp ( _llv / 33.1 ) ;
            _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
            _lmTau = 1.0 / ( _lmAlpha + _lmBeta ) ;
            p_1[i] = p_1[i] + (1. - exp(dt*(( ( ( - 1.0 ) ) ) / _lmTau)))*(- ( ( ( _lmInf ) ) / _lmTau ) / ( ( ( ( - 1.0) ) ) / _lmTau ) - p_1[i]) ;
        }
    }

    void current(){
        double* __restrict p_0 =  (double *)( &c.front()+0*chunk());
        double* __restrict p_1 =  (double *)( &c.front()+1*chunk());

        double* __restrict pVEC_V =  (double *)( &VEC_V.front());
        double* __restrict pVEC_RHS =  (double *)( &VEC_RHS.front());
        double* __restrict pVEC_D =  (double *)( &VEC_D.front());

        int* __restrict pni = (int *)(&ni.front());

        size_t size =  number_instance();
        double lgIh, lihcn, rhs, g, v;

        //PRAGMA_FOR_VECTOR_LOOP
        #pragma acc kernels
        for (int i = 0; i < size; ++i){
            v = pVEC_V[pni[i]];
            lgIh = p_0[i] * p_1[i] ;
            lihcn = lgIh * ( v - ehcn_Ih ) ; //ehcn_Ih is global constant
            rhs = lihcn;
            g = lgIh;
            pVEC_RHS[pni[i]] -= rhs;
            pVEC_D[pni[i]] += g;
        }
    }
};
