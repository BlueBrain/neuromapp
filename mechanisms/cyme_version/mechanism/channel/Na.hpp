#ifndef COREBLURON_CHANNEL_NA_HPP
#define COREBLURON_CHANNEL_NA_HPP

namespace mechanism{
    namespace channel{

        struct Na{
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
                C[mAlpha] = (0.182*(C[v]+35.0))/(1.-(exp((-35.0-C[v])/9.0)));
                C[mBeta]  = (-0.124*(C[v]+35.0))/(1.-(exp((C[v]+35.0)/9.0)));
                C[mInf]   = C[mAlpha]/(C[mAlpha]+C[mBeta]);
                C[mTau]   = 1./(C[mAlpha]+C[mBeta]);
                C[hAlpha] = (0.024*(C[v]+50.0))/(1.-(exp((-50.0-C[v])/5.0)));
                C[hBeta]  = (-0.0091*(C[v]+75.0))/(1.-(exp((C[v]+75.0)/5.0)));
                C[hInf]   = 1./(1.+exp((C[v]+65.0)/6.2));
                C[hTau]   = 1./(C[hAlpha]+C[hBeta]);
            };

            template<class T>
            static inline void cnrn_states(typename T::storage_type& C){
              typedef typename T::storage_type::value_type value_type; //basic float or double
              cnrn_rates<T>(C);
              C[m] += (1.-exp(-corebluron::time<value_type>::dt()/C[mTau]))*(C[mInf]-C[m]);
              C[h] += (1.-exp(-corebluron::time<value_type>::dt()/C[hTau]))*(C[hInf]-C[h]);
            }
        };
/*
            template<class iterator, memory::order O>
            static inline  cyme::serial<typename std::iterator_traits<iterator>::value_type::value_type,O> cnrn_current(
                iterator it,
                typename std::iterator_traits<iterator>::value_type::value_type t = typename std::iterator_traits<iterator>::value_type::value_type() ){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
                    C[v] = t;
                    C[2] = C[0] * C[m] * C[m] * C[m] * C[h];
                    C[1] = C[2] * (C[v] - C[5]);
                    return cyme::serial<value_type,O> (C[1]);
            }

            template<class iterator, memory::order O>
            static inline void cnrn_initmodel(iterator it){
                C[h] = 0. ; // TO DO, C[h] ^= C[h] XOR, or basic set up to 0 <--------------------------------- TIM
                C[m] = 0. ; // C[i] = C[j] kills the persd find a sol
                cnrn_rates(it);
                C[m] = C[mInf]; // TO DO investigate remove = by move ?
                C[h] = C[hInf];
            }

            template<class iterator, memory::order O>
            static inline void cnrn_cur(iterator it){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
                value_type _v=-33.33; // _v voltage fron a node random number
                cyme::serial<value_type,O> tmp  = cnrn_current<iterator, O>(it, _v + 0.001);
                cyme::serial<value_type,O> tmp2 = cnrn_current<iterator, O>(it, _v);
                C[17] = (tmp()-tmp2())/0.001;
            }
            template<class iterator>
            static inline void cnrn_state(iterator it){

             cnrn_states(it);
            }
  */

    } //end namespace channel
} //end namespace mechanism

#endif
