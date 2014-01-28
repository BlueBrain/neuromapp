#include <regression/test_header.hpp>

#include "mechanism/channel/Na.hpp"

using namespace cyme::test;

/* forward declaration */

int rates( double *_p);
int states( double *_p); 
void initmodel(double *_p);
double nrn_cur(double *_p, int num_mechs);

BOOST_AUTO_TEST_CASE_TEMPLATE(init_model_test, T, test_types){

    int size = mechanism::channel::Na::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::channel::Na, typename T::value_type,T::order,T::s> v_cyme(1); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    initmodel(&v_cbluron[0]); // corebluron version
   
    typename corebluron::pack<mechanism::channel::Na,typename T::value_type,T::order,T::s>::iterator it = v_cyme.begin(); 

    mechanism::channel::Na::cnrn_initmodel<typename corebluron::pack<mechanism::channel::Na,typename T::value_type, T::order, T::s>::iterator,T::order>(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(rates_test, T, test_types){

    int size = mechanism::channel::Na::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::channel::Na, typename T::value_type,T::order,T::s> v_cyme(1); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    rates(&v_cbluron[0]); // corebluron version
   
    typename corebluron::pack<mechanism::channel::Na,typename T::value_type,T::order,T::s>::iterator it = v_cyme.begin(); 

    mechanism::channel::Na::cnrn_rates(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(states_test, T, test_types){

    int size = mechanism::channel::Na::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::channel::Na, typename T::value_type,T::order,T::s> v_cyme(1); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    states(&v_cbluron[0]); // corebluron version
   
    typename corebluron::pack<mechanism::channel::Na,typename T::value_type,T::order,T::s>::iterator it = v_cyme.begin(); 

    mechanism::channel::Na::cnrn_states(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(nrn_cur_test, T, test_types){

    int size = mechanism::channel::Na::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::channel::Na, typename T::value_type,T::order,T::s> v_cyme(1); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    nrn_cur(&v_cbluron[0],1); // corebluron version
   
    typedef typename corebluron::pack<mechanism::channel::Na,typename T::value_type,T::order,T::s>::iterator it_def; 
    it_def it = v_cyme.begin(); 

    mechanism::channel::Na::cnrn_cur<it_def,T::order>(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());
}

