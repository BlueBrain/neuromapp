#pragma once

class Na : public mechanism{
public:
    static const int width = 10;
    static std::string name(){return "Na";};

    Na(size_t s, size_t l): mechanism(s,l){}

    void state(){
        double* __restrict p_0 =  (double *)( &c.front()+0*chunk());
        double* __restrict p_1 =  (double *)( &c.front()+1*chunk());
        double* __restrict p_2 =  (double *)( &c.front()+2*chunk());
        double* __restrict p_3 =  (double *)( &c.front()+3*chunk());

        double* __restrict pion_data0 =  (double *)( &ion_data.front()+0*chunk());

        int * __restrict ppvar0 =  (int *)( &ppvar.front()+0*chunk());

        double* __restrict pVEC_V =  (double *)( &VEC_V.front());

        size_t size =  number_instance();

        double dt = 0.1;
        double _lmAlpha , _lmBeta , _lmInf , _lmTau , _lhAlpha , _lhBeta , _lhInf , _lhTau , _llv , _lqt, _v ;

        _lqt = 2.952882641412121 ;

        PRAGMA_FOR_VECTOR_LOOP
        for (int i = 0; i < size; i++) {
            _v = pVEC_V[i];
            _llv = _v;
            p_3[i] = pion_data0[ppvar0[i]];
            if ( _llv == - 32.0 ) {
                _llv = _llv + 0.0001 ;
            }
            _lmAlpha = ( 0.182 * ( _llv - - 32.0 ) ) / ( 1.0 - ( std::exp ( - ( _llv - - 32.0 ) / 6.0 ) ) );
            _lmBeta = ( 0.124 * ( - _llv - 32.0 ) ) / ( 1.0 - ( std::exp ( - ( - _llv - 32.0 ) / 6.0 ) ) );
            _lmInf = _lmAlpha / ( _lmAlpha + _lmBeta ) ;
            _lmTau = ( 1.0 / ( _lmAlpha + _lmBeta ) ) / _lqt ;
            p_0[i] = p_0[i] + (1. - std::exp(dt*(( ( ( - 1.0 ) ) ) / _lmTau)))*(- ( ( ( _lmInf ) ) / _lmTau ) / ( ( ( ( - 1.0) ) ) / _lmTau ) - p_0[i]) ;
            if ( _llv == - 60.0 ) {
                _llv = _llv + 0.0001 ;
            }
            _lhAlpha = ( - 0.015 * ( _llv - - 60.0 ) ) / ( 1.0 - ( std::exp ( ( _llv - - 60.0 ) / 6.0 ) ) );
            _lhBeta = ( - 0.015 * ( - _llv - 60.0 ) ) / ( 1.0 - ( std::exp ( ( - _llv - 60.0 ) / 6.0 ) ) );
            _lhInf = _lhAlpha / ( _lhAlpha + _lhBeta ) ;
            _lhTau = ( 1.0 / ( _lhAlpha + _lhBeta ) ) / _lqt ;
            p_2[i] = p_2[i] + (1. - std::exp(dt*(( ( ( - 1.0 ) ) ) / _lhTau)))*(- ( ( ( _lhInf ) ) / _lhTau ) / ( ( ( ( - 1.0) ) ) / _lhTau ) - p_2[i]) ;
            _v = _llv ;
        }

    }

    void current(){
        double* __restrict p_0 =  (double *)( &c.front()+0*chunk());
        double* __restrict p_1 =  (double *)( &c.front()+1*chunk());
        double* __restrict p_2 =  (double *)( &c.front()+2*chunk());
        double* __restrict p_3 =  (double *)( &c.front()+3*chunk());

        int * __restrict ppvar0 =  (int *)( &ppvar.front()+0*chunk());
        int * __restrict ppvar1 =  (int *)( &ppvar.front()+1*chunk());
        int * __restrict ppvar2 =  (int *)( &ppvar.front()+2*chunk());

        double* __restrict pion_data0 =  (double *)( &ion_data.front()+0*chunk());
        double* __restrict pion_data1 =  (double *)( &ion_data.front()+1*chunk());
        double* __restrict pion_data2 =  (double *)( &ion_data.front()+2*chunk());

        double* __restrict pVEC_V =  (double *)( &VEC_V.front());
        double* __restrict pVEC_RHS =  (double *)( &VEC_RHS.front());

        int* __restrict pni = (int *)(&ni.front());

        double rhs, dina;
        size_t size =  number_instance();

        double _rhs, _g, _v, v;

        PRAGMA_FOR_VECTOR_LOOP
        for (int i = 0; i < size; i++) {
            _v = pVEC_V[pni[i]];
            p_3[i] = pion_data0[ppvar0[i]];
            {
                double _lgNaTs2_t , _lina ;
                _lgNaTs2_t = p_0[i] * p_1[i] * p_1[i] * p_1[i] * p_2[i];
                _lina = _lgNaTs2_t * ( _v - p_3[i]) ;
                _rhs = _lina;
                _g = _lgNaTs2_t;
                pion_data2[ppvar2[i]] += _lgNaTs2_t;
                pion_data1[ppvar1[i]] += _lina ;
            }
            pVEC_RHS[pni[i]] -= _rhs;
            pVEC_V[pni[i]] += _g;
        }
    }
};
