#include <regression/test_header.hpp>

#include "mechanism/synapse/ProbAMPANMDA_EMC.hpp"
#include "mechanism/synapse/ProbGABAAB_EMS.hpp"

using namespace cyme::test;

/* forward declaration */

int state(double *_p); 

// c - __MECHANISM__SYNAPSE__ is given by cmake 

BOOST_AUTO_TEST_CASE_TEMPLATE(states_test, T, test_types){
    int size = mechanism::synapse::__MECHANISM__SYNAPSE__::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::synapse::__MECHANISM__SYNAPSE__, typename T::value_type,T::order,T::s> v_cyme(1); // pack 1 synapse
    
    init(v_cyme,v_cbluron);

    state(&v_cbluron[0]); // corebluron version
   
    typedef typename corebluron::pack<mechanism::synapse::__MECHANISM__SYNAPSE__,typename T::value_type,T::order,T::s>::container_value container;
    typename container::iterator it = v_cyme.begin(); 
    mechanism::synapse::__MECHANISM__SYNAPSE__::cnrn_state<container>(*it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());
}
