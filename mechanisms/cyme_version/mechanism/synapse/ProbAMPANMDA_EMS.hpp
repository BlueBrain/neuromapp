#ifndef SOREBLURON_SYNAPSE_PROBAMPANMDA_EMS_HPP
#define SOREBLURON_SYNAPSE_PROBAMPANMDA_EMS_HPP

namespace mechanism{
    namespace synapse{

        struct ProbAMPANMDA_EMS{
            private:
            enum properties {
                tau_r_AMPA, tau_d_AMPA, tau_r_NMDA, tau_d_NMDA, A_AMPA, B_AMPA, A_NMDA, B_NMDA
            };

            public:
            const static int value_size = 8;

            template<class T>
            static inline void cnrn_functions(typename T::storage_type& S){
                cnrn_state(S);
            }

            template<class T>
            static inline void cnrn_state(typename T::storage_type& S){
                typedef typename T::storage_type::value_type value_type; //basic float or double
                S[A_AMPA] += (1.-exp(-corebluron::time<value_type>::dt()/S[tau_r_AMPA]))*(-S[A_AMPA]);
                S[B_AMPA] += (1.-exp(-corebluron::time<value_type>::dt()/S[tau_d_AMPA]))*(-S[B_AMPA]);
                S[A_NMDA] += (1.-exp(-corebluron::time<value_type>::dt()/S[tau_r_NMDA]))*(-S[A_NMDA]);
                S[B_NMDA] += (1.-exp(-corebluron::time<value_type>::dt()/S[tau_d_NMDA]))*(-S[B_NMDA]);
            }
        };

    }// end namespace synapse
}

/*
            template<class iterator, memory::order O>
            static inline void cnrn_initmodel(iterator it){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type;
                S[21] = S[9];
                S[A_AMPA] = 0.0;
                S[B_AMPA] = 0.0;
                S[A_NMDA] = 0.0;
                S[B_NMDA] = 0.0;
                S[19] = 1.0;
                S[20] = 0.0;
                cyme::serial<value_type,O> _ltp_AMPA ( (S[tau_r_AMPA]*S[tau_d_AMPA]) / (S[tau_d_AMPA]-S[tau_r_AMPA]) * log(S[tau_d_AMPA]/S[tau_r_AMPA]));
                cyme::serial<value_type,O> _ltp_NMPA ( (S[tau_r_NMDA]*S[tau_d_NMDA]) / (S[tau_d_NMDA]-S[tau_r_NMDA]) * log(S[tau_d_NMDA]/S[tau_r_NMDA]));
                S[26]  = 1. / (exp(-_ltp_AMPA()/S[tau_d_AMPA]) - exp (-_ltp_AMPA() / (S[tau_r_AMPA])));
                S[27]  = 1. / (exp(-_ltp_NMPA()/S[tau_d_NMDA]) - exp (-_ltp_NMPA() / (S[tau_r_NMDA])));
            }

            template<class iterator, memory::order O>
            static inline  cyme::serial<typename std::iterator_traits<iterator>::value_type::value_type,O> cnrn_current(
                iterator it,
                typename std::iterator_traits<iterator>::value_type::value_type t = typename std::iterator_traits<iterator>::value_type::value_type()){
                typedef typename std::iterator_traits<iterator>::value_type::value_type value_type; //basic float or double

                S[32] = t;
                cyme::serial<value_type,O> gmax(1); // my value
                cyme::serial<value_type,O> mggate(1.0/(1.0+exp(-0.062*S[32])*(S[8]/3.57)));
                S[16] = gmax() * (S[B_AMPA] - S[A_AMPA]);
                S[17] = gmax() * (S[B_NMDA] - S[A_NMDA]) * mggate();
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
