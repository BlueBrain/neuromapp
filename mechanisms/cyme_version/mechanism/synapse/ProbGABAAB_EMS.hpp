#ifndef SOREBLURON_SYNAPSE_PROBGABAAB_EMS_HPP
#define SOREBLURON_SYNAPSE_PROBGABAAB_EMS_HPP

namespace mechanism{
    namespace synapse{

        struct ProbGABAAB_EMS{
            const static int value_size = 39;

            template<class T>
            static inline void cnrn_functions(typename T::storage_type& S){
                cnrn_state(S);
            }

            template<class T>
            static inline void cnrn_state(typename T::storage_type& S){
                S[26] *= S[18];
                S[27] *= S[19];
                S[28] *= S[20];
                S[29] *= S[21];
            }
        };
    }// end namespace synapse
}// end namespace mechanism
#endif
