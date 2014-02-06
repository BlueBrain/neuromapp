#ifndef COREBLURON_CHANNEL_NATS2_T_HPP
#define COREBLURON_CHANNEL_NATS2_T_HPP

namespace mechanism{
    namespace channel{

        struct NaTs2_t{
            const static int value_size = 18;
       
            template<class T> 
            static inline void cnrn_functions(typename T::storage_type& C){
                cnrn_states<T>(C);
            }

            template<class T> 
            static inline void cnrn_rates(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                value_type _lqt(2.952882641412121); //pow( 2.3 , ( ( 34.0 - 21.0 ) / 10.0 ) ) ;
                C[8] = ( 0.182 * (C[16]+ 32.0 ) ) / ( 1.0 - ( exp ( - (C[16]+ 32.0 ) / 6.0 ) ) ) ;
                C[9] = ( 0.124 * ( -C[16]- 32.0 ) ) / ( 1.0 - ( exp ( - ( -C[16]- 32.0 ) / 6.0 ) ) ) ;
                C[6] = C[8] / ( C[8] + C[9] ) ;
                C[7] = ( 1.0 / ( C[8] + C[9] ) ) / _lqt ;
                C[12] = ( - 0.015 * (C[16]+ 60.0 ) ) / ( 1.0 - ( exp ( (C[16]+ 60.0 ) / 6.0 ) ) ) ;
                C[13] = ( - 0.015 * ( -C[16]- 60.0 ) ) / ( 1.0 - ( exp ( ( -C[16]- 60.0 ) / 6.0 ) ) ) ;
                C[10] = C[12] / ( C[12] + C[13] ) ;
                C[11] = ( 1.0 / ( C[12] + C[13] ) ) / _lqt ;
            }

            template<class T> 
            static inline void cnrn_states(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                cnrn_rates<T>(C);
                C[3] += (1. - exp(corebluron::time<value_type>::dt()*(( ( ( - 1.0 ) ) ) / C[7])))*(- ( ( ( C[6] ) ) / C[7] ) / ( ( ( ( - 1.0) ) ) / C[7] ) -C[3]);
                C[4] += (1. - exp(corebluron::time<value_type>::dt()*(( ( ( - 1.0 ) ) ) / C[11])))*(- ( ( ( C[10] ) ) / C[11] ) / ( ( ( ( - 1.0) ) ) / C[11] ) -C[4]);
            }
        
        };

    } //end namespace channel
} //end namespace mechanism

#endif
