#ifndef COREBLURON_SYNAPSE_PROBAMPANMDA_EMC_HPP
#define COREBLURON_SYNAPSE_PROBAMPANMDA_EMC_HPP

namespace mechanism{
    namespace synapse{
    
        struct ProbAMPANMDA{
            const static int value_size = 35; 

            template<class iterator, memory::order O>
            static inline void cnrn_functions(iterator it){
//                cnrn_initmodel<iterator,O>(it);
                cnrn_cur<iterator,O>(it);
                cnrn_state(it);
            }
                
            template<class iterator>
            static inline void cnrn_states(iterator it){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
                    (*it)[22] += (1. - exp(corebluron::time<value_type>::dt()*(-1.)/(*it)[0])) * (-(*it)[22]);
                    (*it)[23] += (1. - exp(corebluron::time<value_type>::dt()*(-1.)/(*it)[1])) * (-(*it)[23]);
                    (*it)[24] += (1. - exp(corebluron::time<value_type>::dt()*(-1.)/(*it)[2])) * (-(*it)[24]);
                    (*it)[25] += (1. - exp(corebluron::time<value_type>::dt()*(-1.)/(*it)[3])) * (-(*it)[25]);
            }

            template<class iterator, memory::order O>
            static inline void cnrn_initmodel(iterator it){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type;
                (*it)[21] = (*it)[9];
                (*it)[22] = 0.0;
                (*it)[23] = 0.0;
                (*it)[24] = 0.0;
                (*it)[25] = 0.0;
                (*it)[19] = 1.0;
                (*it)[20] = 0.0;
                cyme::serial<value_type,O> _ltp_AMPA ( ((*it)[0]*(*it)[1]) / ((*it)[1]-(*it)[0]) * log((*it)[1]/(*it)[0]));
                cyme::serial<value_type,O> _ltp_NMPA ( ((*it)[2]*(*it)[3]) / ((*it)[3]-(*it)[2]) * log((*it)[3]/(*it)[2]));
                (*it)[26]  = 1. / (exp(-_ltp_AMPA()/(*it)[1]) - exp (-_ltp_AMPA() / ((*it)[0])));
                (*it)[27]  = 1. / (exp(-_ltp_NMPA()/(*it)[3]) - exp (-_ltp_NMPA() / ((*it)[2])));
            }
      
            template<class iterator, memory::order O>
            static inline  cyme::serial<typename std::iterator_traits<iterator>::value_type::value_type,O> cnrn_current(
                iterator it, /* arg 0 */
                typename std::iterator_traits<iterator>::value_type::value_type t = typename std::iterator_traits<iterator>::value_type::value_type() /* arg 1 */){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
               
                (*it)[32] = t;
                cyme::serial<value_type,O> gmax(1); // my value
                cyme::serial<value_type,O> mggate(1.0/(1.0+exp(-0.062*(*it)[32])*((*it)[8]/3.57)));
                (*it)[16] = gmax() * ((*it)[23] - (*it)[22]);
                (*it)[17] = gmax() * ((*it)[25] - (*it)[24]) * mggate();
                (*it)[18] = (*it)[16] + (*it)[17];
                (*it)[14] = (*it)[16]*((*it)[32]-(*it)[7]);
                (*it)[15] = (*it)[17]*((*it)[32]-(*it)[7]);
                (*it)[13] = (*it)[14]+(*it)[15];
                return cyme::serial<value_type,O>((*it)[13]);
            }

            template<class iterator, memory::order O>
            static inline void cnrn_cur(iterator it){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
                value_type _v=0.; // _v voltage fron a node random number
                value_type _nd_area=3.14; // _v voltage fron a node random number
                cyme::serial<value_type,O> tmp  = cnrn_current<iterator, O>(it, _v + 0.001);
                cyme::serial<value_type,O> tmp2 = cnrn_current<iterator, O>(it, _v);
                (*it)[33] = (tmp()-tmp2())/0.001*(1.e2/_nd_area);
             //   tmp2() = tmp2()*1.e2/_nd_area 
            }

            template<class iterator>
            static inline void cnrn_state(iterator it){
                cnrn_states(it);
            }
        }; 
    }// end namespace synapse
}// end namespace mechanism
 
#endif
