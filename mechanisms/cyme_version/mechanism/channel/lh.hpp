
#ifndef COREBLURON_CHANNEL_LH_HPP
#define COREBLURON_CHANNEL_LH_HPP

namespace mechanism{
    namespace channel{

        struct lh{
            const static int value_size = 11;

            template<class T>
            static inline void cnrn_functions(typename T::storage_type& C){
                cnrn_states<T>(C);
            }

            template<class T>
            static inline void cnrn_rates(typename T::storage_type& C){
                C[6] = 0.00643 * (C[9]+ 154.9 ) / ( exp ( (C[9]+ 154.9 ) / 11.9 ) - 1.0 ) ;
                C[7] = 0.193 * exp (C[9]/ 33.1 ) ;
                C[4] = C[6] / ( C[6] + C[7] ) ;
                C[5] = 1.0 / ( C[6] + C[7] ) ;
            };

            template<class T>
            static inline void cnrn_states(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                cnrn_rates<T>(C);
                C[3] += (1. - exp(corebluron::time<value_type>::dt()*(( ( ( - 1.0 ) ) ) / C[5])))*(- ( ( ( C[4] ) ) / C[5] ) / ( ( ( ( - 1.0) ) ) / C[5] ) - C[3]) ;
            }
        };

    } //end namespace channel
} //end namespace mechanism

#endif
