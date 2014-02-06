#ifndef COREBLURON_CHANNEL_NA_HPP
#define COREBLURON_CHANNEL_NA_HPP

namespace mechanism{
    namespace channel{

        struct Na{
            const static int value_size = 18;
 
            template<class T>
            static inline void cnrn_functions(typename T::storage_type& C){
                cnrn_states<T>(C);
            }

            template<class T>
            static inline void cnrn_rates(typename T::storage_type& C){
                C[8]  = (0.182*(C[16]+35.0)) / (1.-(exp((-35.0 - C[16])/9.0)));
                C[9]  = (-0.124*(C[16]+35.0))/ (1.-(exp((C[16]+35.0)/9.0)));
                C[6]  = C[8]/(C[8]+C[9]);
                C[7]  = 1./(C[8]+C[9]);
                C[12] = (0.024*(C[16]+50.0))  /(1.-(exp((-50.0 -C[16])/5.0)));
                C[13] = (-0.0091*(C[16]+75.0))/(1.-(exp((C[16]+75.0)/5.0)));
                C[10] = 1./(1.+exp((C[16]+65.0)/6.2));
                C[11] = 1./(C[12]+C[13]);
            };

            template<class T>
            static inline void cnrn_states(typename T::storage_type& C){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                cnrn_rates<T>(C);
                C[3] += (1.-exp(corebluron::time<value_type>::dt()*(-1./C[7] )))*((C[6] /C[7]) /(1./C[7]) -C[3]);
                C[4] += (1.-exp(corebluron::time<value_type>::dt()*(-1./C[11])))*((C[10]/C[11])/(1./C[11])-C[4]);
            }
        };
/*
            template<class iterator, memory::order O>
            static inline  cyme::serial<typename std::iterator_traits<iterator>::value_type::value_type,O> cnrn_current(
                iterator it, 
                typename std::iterator_traits<iterator>::value_type::value_type t = typename std::iterator_traits<iterator>::value_type::value_type() ){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
                    C[16] = t;
                    C[2] = C[0] * C[3] * C[3] * C[3] * C[4];
                    C[1] = C[2] * (C[16] - C[5]);
                    return cyme::serial<value_type,O> (C[1]);
            }
        
            template<class iterator, memory::order O>
            static inline void cnrn_initmodel(iterator it){
                C[4] = 0. ; // TO DO, C[4] ^= C[4] XOR, or basic set up to 0 <--------------------------------- TIM
                C[3] = 0. ; // C[i] = C[j] kills the persd find a sol
                cnrn_rates(it);
                C[3] = C[6]; // TO DO investigate remove = by move ? 
                C[4] = C[10]; 
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
