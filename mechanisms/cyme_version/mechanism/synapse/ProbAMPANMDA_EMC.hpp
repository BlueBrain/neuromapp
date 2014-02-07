#ifndef SOREBLURON_SYNAPSE_PROBAMPANMDA_EMS_HPP
#define SOREBLURON_SYNAPSE_PROBAMPANMDA_EMS_HPP

namespace mechanism{
    namespace synapse{

        struct ProbAMPANMDA_EMS{
            const static int value_size = 35;

            template<class T>
            static inline void cnrn_functions(typename T::storage_type& S){
                cnrn_state(S);
            }

            template<class T>
            static inline void cnrn_state(typename T::storage_type& S){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                S[22] += (1. - exp(corebluron::time<value_type>::dt()*(-1.)/S[0])) * (-S[22]);
                S[23] += (1. - exp(corebluron::time<value_type>::dt()*(-1.)/S[1])) * (-S[23]);
                S[24] += (1. - exp(corebluron::time<value_type>::dt()*(-1.)/S[2])) * (-S[24]);
                S[25] += (1. - exp(corebluron::time<value_type>::dt()*(-1.)/S[3])) * (-S[25]);
            }
        };

    }// end namespace synapse
}

/*
            template<class iterator, memory::order O>
            static inline void cnrn_initmodel(iterator it){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type;
                S[21] = S[9];
                S[22] = 0.0;
                S[23] = 0.0;
                S[24] = 0.0;
                S[25] = 0.0;
                S[19] = 1.0;
                S[20] = 0.0;
                cyme::serial<value_type,O> _ltp_AMPA ( (S[0]*S[1]) / (S[1]-S[0]) * log(S[1]/S[0]));
                cyme::serial<value_type,O> _ltp_NMPA ( (S[2]*S[3]) / (S[3]-S[2]) * log(S[3]/S[2]));
                S[26]  = 1. / (exp(-_ltp_AMPA()/S[1]) - exp (-_ltp_AMPA() / (S[0])));
                S[27]  = 1. / (exp(-_ltp_NMPA()/S[3]) - exp (-_ltp_NMPA() / (S[2])));
            }

            template<class iterator, memory::order O>
            static inline  cyme::serial<typename std::iterator_traits<iterator>::value_type::value_type,O> cnrn_current(
                iterator it,
                typename std::iterator_traits<iterator>::value_type::value_type t = typename std::iterator_traits<iterator>::value_type::value_type()){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double

                S[32] = t;
                cyme::serial<value_type,O> gmax(1); // my value
                cyme::serial<value_type,O> mggate(1.0/(1.0+exp(-0.062*S[32])*(S[8]/3.57)));
                S[16] = gmax() * (S[23] - S[22]);
                S[17] = gmax() * (S[25] - S[24]) * mggate();
                S[18] = S[16] + S[17];
                S[14] = S[16]*(S[32]-S[7]);
                S[15] = S[17]*(S[32]-S[7]);
                S[13] = S[14]+S[15];
                return cyme::serial<value_type,O>(S[13]);
            }

            template<class iterator, memory::order O>
            static inline void cnrn_cur(iterator it){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double
                value_type _v=0.; // _v voltage fron a node random number
                value_type _nd_area=3.14; // _v voltage fron a node random number
                cyme::serial<value_type,O> tmp  = cnrn_current<iterator, O>(it, _v + 0.001);
                cyme::serial<value_type,O> tmp2 = cnrn_current<iterator, O>(it, _v);
                S[33] = (tmp()-tmp2())/0.001*(1.e2/_nd_area);
             //   tmp2() = tmp2()*1.e2/_nd_area
            }
*/
// end namespace mechanism

#endif
