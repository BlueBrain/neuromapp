#ifndef SOREBLURON_SYNAPSE_PROBGABAAB_EMS_HPP
#define SOREBLURON_SYNAPSE_PROBGABAAB_EMS_HPP

namespace mechanism{
    namespace synapse{

        struct ProbGABAAB_EMS{
            private:
            enum properties {
                A_GABAA_step, B_GABAA_step, A_GABAB_step, B_GABAB_step, A_GABAA, B_GABAA, A_GABAB, B_GABAB
            };

            public:
            const static int value_size = 8;

            template<class T>
            static inline void cnrn_functions(typename T::storage_type& S){
                cnrn_state(S);
            }

            template<class T>
            static inline void cnrn_state(typename T::storage_type& S){
                S[A_GABAA] *= S[A_GABAA_step];
                S[B_GABAA] *= S[B_GABAA_step];
                S[A_GABAB] *= S[A_GABAB_step];
                S[B_GABAB] *= S[B_GABAB_step];
            }
        };
    }// end namespace synapse
}// end namespace mechanism
#endif
