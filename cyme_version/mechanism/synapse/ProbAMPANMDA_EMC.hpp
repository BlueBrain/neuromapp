#ifndef COREBLURON_SYNAPSE_PROBAMPANMDA_EMC_HPP
#define COREBLURON_SYNAPSE_PROBAMPANMDA_EMC_HPP

namespace mechanism{
    namespace synapse{
    
        struct ProbAMPANMDA{
            const static int value_size = 35; 
                
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
                (*it)[22] = 0.0;
                (*it)[23] = 0.0;
                (*it)[23] = 0.0;
                (*it)[24] = 0.0;
                (*it)[19] = 1.0;
                (*it)[20] = 0.0;
//              cyme::serial<value_type,O> _ltp_AMPA = ((*it)[0]*(*it)[1]) / ((*it)[1]-(*it)[0]) ; // /log((*it)[1]/(*it)[0])) <----------- TO code log
                cyme::serial<value_type,O> _ltp_AMPA ( ((*it)[0]*(*it)[1]) / ((*it)[1]-(*it)[0]) ); // /log((*it)[1]/(*it)[0])) <----------- TO code log
                cyme::serial<value_type,O> _ltp_NMPA ( ((*it)[2]*(*it)[3]) / ((*it)[2]-(*it)[3]) ); // /log((*it)[3]/(*it)[2])) <----------- TO code log
                (*it)[26]  = 1. / exp(-_ltp_AMPA()/(*it)[0]) - exp ( _ltp_AMPA() / (-(*it)[1]));
                (*it)[27]  = 1. / exp(-_ltp_AMPA()/(*it)[2]) - exp ( _ltp_AMPA() / (-(*it)[3]));
            }

            }; 
    }// end namespace synapse
}// end namespace mechanism
 
#endif
