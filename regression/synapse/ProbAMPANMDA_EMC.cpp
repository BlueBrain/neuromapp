#include <regression/test_header.hpp>

#include "mechanism/synapse/ProbAMPANMDA_EMC.hpp"

using namespace cyme::test;

/* forward declaration */

int rates( double *_p);
int states( double *_p);
void initmodel(double *_p);

double nrn_cur(double *_p, int num_mechs);

BOOST_AUTO_TEST_CASE(init_model_test){

    int size = mechanism::synapse::ProbAMPANMDA::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::synapse::ProbAMPANMDA,double,memory::AoSoA> v_cyme(1,0); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    initmodel(&v_cbluron[0]); // corebluron version
    typedef corebluron::pack<mechanism::synapse::ProbAMPANMDA,double,memory::AoSoA>::iterator it_type;  
    it_type it = v_cyme.begin(); 

    mechanism::synapse::ProbAMPANMDA::cnrn_initmodel<it_type,memory::AoSoA>(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), 0.0001);
}

BOOST_AUTO_TEST_CASE(states_test){

    int size = mechanism::synapse::ProbAMPANMDA::value_size;

    std::vector<double> v_cbluron(size);
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);
    corebluron::pack<mechanism::synapse::ProbAMPANMDA,double,memory::AoSoA> v_cyme(1,0); // pack 1 channel

    init(v_cyme,v_cbluron);

    states(&v_cbluron[0]); // corebluron version

    corebluron::pack<mechanism::synapse::ProbAMPANMDA,double,memory::AoSoA>::iterator it = v_cyme.begin();

    mechanism::synapse::ProbAMPANMDA::cnrn_states(it);

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), 0.0001);
}
