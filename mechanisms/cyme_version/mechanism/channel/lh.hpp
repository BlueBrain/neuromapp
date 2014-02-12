
#ifndef COREBLURON_CHANNEL_LH_HPP
#define COREBLURON_CHANNEL_LH_HPP


namespace mechanism{
    namespace channel{

        struct lh{
            private:
            enum properties {
                m, mInf, mTau, mAlpha, mBeta, v
            };

            public:
            const static int value_size = 6;

            template<class T>
            static inline void cnrn_functions(typename T::storage_type& C){
                cnrn_states<T>(C);
            }

            template<class T>
            static inline void cnrn_rates(typename T::storage_type& C){
                C[mAlpha] = 0.00643*(C[v]+154.9)/(exp((C[v]+154.9)/11.9)-1.0);
                C[mBeta] = 0.193*exp(C[v]/33.1);
                C[mInf] = C[mAlpha]/(C[mAlpha]+C[mBeta]);
                C[mTau] = 1.0/(C[mAlpha]+C[mBeta]);
            };

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
