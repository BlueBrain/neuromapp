#include <regression/test_header.hpp>

#include "mechanism/synapse/ProbAMPANMDA_EMC.hpp"

using namespace cyme::test;

/* forward declaration */

int rates( double *_p);
int states( double*_p);
void initmodel( double*_p);

double nrn_cur(double*_p, int num_mechs);

BOOST_AUTO_TEST_CASE_TEMPLATE(init_model_test, T, test_types){
    int size = mechanism::synapse::ProbAMPANMDA::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<typename T::value_type>);     
    corebluron::pack<mechanism::synapse::ProbAMPANMDA,typename T::value_type,T::order> v_cyme(1,0); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    initmodel(&v_cbluron[0]); // corebluron version
    typename corebluron::pack<mechanism::synapse::ProbAMPANMDA,typename T::value_type,T::order>::iterator it = v_cyme.begin(); 

    mechanism::synapse::ProbAMPANMDA::cnrn_initmodel<
                                                      corebluron::pack<mechanism::synapse::ProbAMPANMDA,
                                                      typename T::value_type,
                                                      T::order
                                                    >::iterator,T::order>(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());

}

BOOST_AUTO_TEST_CASE_TEMPLATE(states_test, T, test_types){

    int size = mechanism::synapse::ProbAMPANMDA::value_size;

    std::vector<double> v_cbluron(size);
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<typename T::value_type>);
    corebluron::pack<mechanism::synapse::ProbAMPANMDA,typename T::value_type,T::order> v_cyme(1,0); // pack 1 channel

    init(v_cyme,v_cbluron);

    states(&v_cbluron[0]); // corebluron version

    typename corebluron::pack<mechanism::synapse::ProbAMPANMDA,typename T::value_type,T::order>::iterator it = v_cyme.begin();

    mechanism::synapse::ProbAMPANMDA::cnrn_states(it);

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(nrn_cur_test, T, test_types){

    int size = mechanism::synapse::ProbAMPANMDA::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<typename T::value_type>);     
    corebluron::pack<mechanism::synapse::ProbAMPANMDA,typename T::value_type,T::order> v_cyme(1,0); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    nrn_cur(&v_cbluron[0],1); // corebluron version

    typename corebluron::pack<mechanism::synapse::ProbAMPANMDA,typename T::value_type,T::order>::iterator it = v_cyme.begin(); 

    mechanism::synapse::ProbAMPANMDA::cnrn_cur<
                                                      corebluron::pack<mechanism::synapse::ProbAMPANMDA,
                                                      typename T::value_type,
                                                      T::order
                                                    >::iterator,T::order>(it); 
   
    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());
 
}



