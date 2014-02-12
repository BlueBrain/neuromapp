#ifndef COREBLURON_CHANNEL_NATS2_T_HPP
#define COREBLURON_CHANNEL_NATS2_T_HPP

namespace mechanism{
    namespace channel{

        struct NaTs2_t{
            private:
            enum properties {
                m, h, mInf, mTau, mAlpha, mBeta, hInf, hTau, hAlpha, hBeta, v
            };

            public:
            const static int value_size = 11;

            template<class T>
            static inline void cnrn_functions(typename T::storage_type& C){
                cnrn_states<T>(C);
            }

            template<class T>
            static inline void cnrn_rates(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                value_type _1_on_lqt(0.338652131302374); //1 / pow( 2.3 , ( ( 34.0 - 21.0 ) / 10.0 ) ) ;
                C[mAlpha] = (0.182*(C[v]+32.0))/(1.0-(exp((C[v]+32.0)/-6.0))) ;
                C[mBeta]  = (-0.124*(C[v]+32.0))/(1.0-(exp((C[v]+32.0)/6.0))) ;
                C[mInf]   = C[mAlpha]/(C[mAlpha]+C[mBeta]);
                C[mTau]   = _1_on_lqt/(C[mAlpha]+C[mBeta]);
                C[hAlpha] = (-0.015*(C[v]+60.0))/(1.0-(exp((C[v]+60.0)/6.0)));
                C[hBeta]  = ( 0.015*(C[v]+60.0))/(1.0-(exp((C[v]+60.0)/-6.0))) ;
                C[hInf]   = C[hAlpha]/(C[hAlpha]+C[hBeta]);
                C[hTau]   = _1_on_lqt/(C[hAlpha]+C[hBeta]);
            }

            template<class T>
            static inline void cnrn_states(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                cnrn_rates<T>(C);
                C[m] += (1.-exp(-corebluron::time<value_type>::dt()/C[mTau]))*(C[mInf]-C[m]);
                C[h] += (1.-exp(-corebluron::time<value_type>::dt()/C[hTau]))*(C[hInf]-C[h]);
            }
        };

    } //end namespace channel
} //end namespace mechanism

#endif
