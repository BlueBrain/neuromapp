#ifndef COREBLURON_CHANNEL_SKV3_1_HPP
#define COREBLURON_CHANNEL_SKV3_1_HPP

namespace mechanism{
    namespace channel{

        struct SKv3_1{
            private:
            enum properties {
                m, mInf, mTau, v
            };

            public:
            const static int value_size = 4;

            template<class T>
            static inline void cnrn_functions(typename T::storage_type& C){
                cnrn_states<T>(C);
            }

            template<class T>
            static inline void cnrn_rates(typename T::storage_type& C){
                C[mInf] = 1/(1+exp((C[v]-18.700)/-9.700));
                C[mTau] = 4/(1+exp((C[v]+46.560)/-44.140));
            }

            template<class T>
            static inline void cnrn_states(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                cnrn_rates<T>(C);
                C[m] += (1.-exp(-corebluron::time<value_type>::dt()/C[mTau]))*(C[mInf]-C[m]);
            }
        };

    } //end namespace channel
} //end namespace mechanism

#endif
