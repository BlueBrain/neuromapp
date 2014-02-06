#ifndef COREBLURON_CHANNEL_SKV3_1_HPP
#define COREBLURON_CHANNEL_SKV3_1_HPP

namespace mechanism{
    namespace channel{

        struct SKv3_1{
            const static int value_size = 10;
        
            template<class T>
            static inline void cnrn_functions(typename T::storage_type& C){
                cnrn_states<T>(C);
            }

            template<class T>
            static inline void cnrn_rates(typename T::storage_type& C){
                C[5] = 1.0 / ( 1.0 + exp ( ( ( C[8] - ( 18.700 ) ) / ( - 9.700 ) ) ) ) ;
                C[6] = 0.2 * 20.000 / ( 1.0 + exp ( ( ( C[8] - ( - 46.560 ) ) / ( - 44.140 ) ) ) ) ;
            }

            template<class T>
            static inline void cnrn_states(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                cnrn_rates<T>(C);
                C[3] +=  (1. - exp(corebluron::time<value_type>::dt()*(( ( ( - 1.0 ) ) ) / C[6])))*(- ( ( ( C[5] ) ) / C[6] ) / ( ( ( ( - 1.0) ) ) / C[6] ) - C[3]) ;
            }
        
        };

    } //end namespace channel
} //end namespace mechanism

#endif
