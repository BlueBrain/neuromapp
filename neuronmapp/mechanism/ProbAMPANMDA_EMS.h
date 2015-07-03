#pragma once

class ProbAMPANMDA_EMS : public mechanism{
public:
    static const int width = 31;
    static std::string name(){return "ProbAMPANMDA_EMS";};
    ProbAMPANMDA_EMS(size_t s, size_t l): mechanism(s,l){}

    void state(){
        double* __restrict p_7 =  (double *)( &c.front()+7*chunk());
        double* __restrict p_8 =  (double *)( &c.front()+8*chunk());
        double* __restrict p_20 =  (double *)( &c.front()+20*chunk());
        double* __restrict p_21 =  (double *)( &c.front()+21*chunk());
        double* __restrict p_22 =  (double *)( &c.front()+22*chunk());
        double* __restrict p_23 =  (double *)( &c.front()+23*chunk());

        double* __restrict ppvar0 =  (double *)( &ppvar.front()+0*chunk());

        double* __restrict pVEC_V =  (double *)( &VEC_V.front());
        double* __restrict pVEC_RHS =  (double *)( &VEC_RHS.front());
        double* __restrict pVEC_D =  (double *)( &VEC_D.front());

        int* __restrict pni = (int *)(&ni.front());

        double* __restrict pshadow_rhs = (double *)(&shadow_rhs.front());
        double* __restrict pshadow_d = (double *)(&shadow_d.front());


        size_t size =  number_instance();
        double _lmggate , _rhs, _g, _lg_AMPA , _lg_NMDA , _lg , _li_AMPA , _li_NMDA , _lvv , _li , _lvve, _v ;
        double gmax_ProbAMPANMDA_EMS(1.1); // random value

        PRAGMA_FOR_VECTOR_LOOP
        for (int i = 0; i < size; ++i) {
            _v = pVEC_V[pni[i]];
            _lvv = _v ;
            _lmggate = 1.0 / ( 1.0 + exp ( 0.062 * - ( _lvv ) ) * ( p_8[i] / 3.57 ) ) ;
            _lg_AMPA = gmax_ProbAMPANMDA_EMS * ( p_21[i] - p_20[i] ) ;
            _lg_NMDA = gmax_ProbAMPANMDA_EMS * ( p_23[i] - p_22[i] ) * _lmggate ;
            _lg = _lg_AMPA + _lg_NMDA ;
            _lvve = ( _lvv - p_7[i] ) ;
            _li_AMPA = _lg_AMPA * _lvve ;
            _li_NMDA = _lg_NMDA * _lvve ;
            _li = _li_AMPA + _li_NMDA ;
            _rhs = _li;
            _g = _lg_AMPA + _lg_NMDA;
            _g *= 1.e2/ppvar0[i];
            _rhs *= 1.e2/ppvar0[i];
            pshadow_rhs[i] = _rhs;
            pshadow_d[i] = _g;
        }

        PRAGMA_FOR_VECTOR_LOOP
        for (int i = 0; i < size; ++i) {
            pVEC_RHS[pni[i]] -= pshadow_rhs[i];
            pVEC_D[pni[i]] +=  pshadow_d[i];
        }
    }

    void current(){
        double* __restrict p_13 =  (double *)( &c.front()+13*chunk());
        double* __restrict p_14 =  (double *)( &c.front()+14*chunk());
        double* __restrict p_15 =  (double *)( &c.front()+15*chunk());
        double* __restrict p_16 =  (double *)( &c.front()+16*chunk());
        double* __restrict p_20 =  (double *)( &c.front()+20*chunk());
        double* __restrict p_21 =  (double *)( &c.front()+21*chunk());
        double* __restrict p_22 =  (double *)( &c.front()+22*chunk());
        double* __restrict p_23 =  (double *)( &c.front()+23*chunk());

        size_t size =  number_instance();

        PRAGMA_FOR_VECTOR_LOOP
        for (int i = 0; i < size ; ++i) {
            p_20[i] *= p_13[i] ;
            p_21[i] *= p_14[i] ;
            p_22[i] *= p_15[i] ;
            p_23[i] *= p_16[i] ;
        }
    }
};
