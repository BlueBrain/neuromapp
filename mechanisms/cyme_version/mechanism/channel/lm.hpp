#ifndef COREBLURON_CHANNEL_LM_HPP
#define COREBLURON_CHANNEL_LM_HPP

namespace mechanism{
    namespace channel{

        struct lm{
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
                typedef typename T::storage_type::value_type value_type; //basic float or double
                value_type _1_on_lqt(0.338652131302374); //1 / pow( 2.3 , ( ( 34.0 - 21.0 ) / 10.0 ) ) ;
                C[mAlpha] = 3.3e-3*exp(0.1*(C[v]+35.0));
                C[mBeta] = 3.3e-3*exp(-0.1*(C[v]+35.0));
                C[mInf] = C[mAlpha]/(C[mAlpha]+C[mBeta]);
                C[mTau] = _1_on_lqt/(C[mAlpha]+C[mBeta]);
            };

            template<class T>
            static inline void cnrn_states(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                cnrn_rates<T>(C);
                C[m] += (1.-exp(-corebluron::time<value_type>::dt()/C[mTau]))*(C[mInf]-C[m]);
            }
        };

   }//end namespace channel
} //end namespace mechanism

#endif
