#ifndef COREBLURON_CHANNEL_NA_HPP
#define COREBLURON_CHANNEL_NA_HPP

namespace mechanism{
    namespace channel{

        struct Na{
            const static int value_size = 18;
        
            template<class iterator, memory::order O>
            static inline void cnrn_functions(iterator it){
                cnrn_initmodel<iterator,O>(it);
                cnrn_cur<iterator,O>(it);
                cnrn_state(it);
            }
        
            template<class iterator>
            static inline void cnrn_rates(iterator it){
                (*it)[8]  = (0.182*((*it)[16]+35.0)) / (1.-(exp((-35.0 - (*it)[16])/9.0)));
                (*it)[9]  = (-0.124*((*it)[16]+35.0))/ (1.-(exp(((*it)[16]+35.0)/9.0)));
                (*it)[6]  = (*it)[8]/((*it)[8]+(*it)[9]);
                (*it)[7]  = 1./((*it)[8]+(*it)[9]);
                (*it)[12] = (0.024*((*it)[16]+50.0))  /(1.-(exp((-50.0 -(*it)[16])/5.0)));
                (*it)[13] = (-0.0091*((*it)[16]+75.0))/(1.-(exp(((*it)[16]+75.0)/5.0)));
                (*it)[10] = 1./(1.+exp(((*it)[16]+65.0)/6.2));
                (*it)[11] = 1./((*it)[12]+(*it)[13]);
            };
        
            template<class iterator>
            static inline void cnrn_states(iterator it){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
                cnrn_rates(it);
                (*it)[3] += (1.-exp(corebluron::time<value_type>::dt()*(-1./(*it)[7] )))*(((*it)[6] /(*it)[7]) /(1./(*it)[7]) -(*it)[3]);
                (*it)[4] += (1.-exp(corebluron::time<value_type>::dt()*(-1./(*it)[11])))*(((*it)[10]/(*it)[11])/(1./(*it)[11])-(*it)[4]);
            }
        
            template<class iterator, memory::order O>
            static inline  cyme::serial<typename std::iterator_traits<iterator>::value_type::value_type,O> cnrn_current(
                iterator it, /* arg 0 */
                typename std::iterator_traits<iterator>::value_type::value_type t = typename std::iterator_traits<iterator>::value_type::value_type() /* arg 1 */){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
                    (*it)[16] = t;
                    (*it)[2] = (*it)[0] * (*it)[3] * (*it)[3] * (*it)[3] * (*it)[4];
                    (*it)[1] = (*it)[2] * ((*it)[16] - (*it)[5]);
                    return cyme::serial<value_type,O> ((*it)[1]);
            }
        
            template<class iterator, memory::order O>
            static inline void cnrn_initmodel(iterator it){
                (*it)[4] = 0. ; // TO DO, (*it)[4] ^= (*it)[4] XOR, or basic set up to 0 <--------------------------------- TIM
                (*it)[3] = 0. ; // (*it)[i] = (*it)[j] kills the persd find a sol
                cnrn_rates(it);
                (*it)[3] = (*it)[6]; // TO DO investigate remove = by move ? 
                (*it)[4] = (*it)[10]; 
            }
        
            template<class iterator, memory::order O>
            static inline void cnrn_cur(iterator it){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
                value_type _v=-33.33; // _v voltage fron a node random number
                cyme::serial<value_type,O> tmp  = cnrn_current<iterator, O>(it, _v + 0.001);
                cyme::serial<value_type,O> tmp2 = cnrn_current<iterator, O>(it, _v);
                tmp -= tmp2; //  TO DO TUNE ME <--------------------------- TIM
                (*it)[17] = tmp;
                (*it)[17] = (*it)[17]/0.001;
            }
            
            template<class iterator>
            static inline void cnrn_state(iterator it){
                cnrn_states(it);
            }
        };

    } //end namespace channel
} //end namespace mechanism

#endif
