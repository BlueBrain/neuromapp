#ifndef COREBLURON_CHANNEL_LM_HPP
#define COREBLURON_CHANNEL_LM_HPP

namespace mechanism{
    namespace channel{

        struct lm{
            const static int value_size = 12;

            template<class T>
            static inline void cnrn_functions(typename T::storage_type& C){
                cnrn_states<T>(C);
            }

            template<class T>
            static inline void cnrn_rates(typename T::storage_type& C){
                double _lqt(2.952882641412121); //_lqt = pow( 2.3 , ( ( 34.0 - 21.0 ) / 10.0 ) ) ;
                C[7] = 3.3e-3*exp(0.1*(C[10]+35.0));
                C[8] = 3.3e-3*exp(-0.1*(C[10]+35.0));
                C[5] = C[7]/(C[7]+C[8]);
                C[6] = (1.0/(C[7]+C[8]))/_lqt;
            };

            template<class T>
            static inline void cnrn_states(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                cnrn_rates<T>(C);
                C[3] += (1.-exp(corebluron::time<value_type>::dt()*(-1.0/C[6])))*(-(C[5]/C[6])/(- 1.0/C[6])-C[3]);
            }
        };

    } //end namespace channel
} //end namespace mechanism

#endif
