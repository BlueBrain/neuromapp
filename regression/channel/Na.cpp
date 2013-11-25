#include <regression/test_header.hpp>

#include "mechanism/channel/Na.hpp"

using namespace cyme::test;

/* forward declaration */

int rates( double *_p);
int states( double *_p); 
void initmodel(double *_p);
double nrn_cur(double *_p, int num_mechs);

BOOST_AUTO_TEST_CASE(init_model_test){

    int size = mechanism::channel::Na::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::channel::Na,double,memory::AoSoA> v_cyme(1,0); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    initmodel(&v_cbluron[0]); // corebluron version
   
    corebluron::pack<mechanism::channel::Na,double,memory::AoSoA>::iterator it = v_cyme.begin(); 

    mechanism::channel::Na::cnrn_initmodel<corebluron::pack<mechanism::channel::Na,double,memory::AoSoA>::iterator,memory::AoSoA>(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), 0.0001);
}

BOOST_AUTO_TEST_CASE(rates_test){

    int size = mechanism::channel::Na::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::channel::Na,double,memory::AoSoA> v_cyme(1,0); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    rates(&v_cbluron[0]); // corebluron version
   
    corebluron::pack<mechanism::channel::Na,double,memory::AoSoA>::iterator it = v_cyme.begin(); 

    mechanism::channel::Na::cnrn_rates(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), 0.0001);
}

BOOST_AUTO_TEST_CASE(states_test){

    int size = mechanism::channel::Na::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::channel::Na,double,memory::AoSoA> v_cyme(1,0); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    states(&v_cbluron[0]); // corebluron version
   
    corebluron::pack<mechanism::channel::Na,double,memory::AoSoA>::iterator it = v_cyme.begin(); 

    mechanism::channel::Na::cnrn_states(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), 0.0001);
}

BOOST_AUTO_TEST_CASE(nrn_cur_test){

    int size = mechanism::channel::Na::value_size;

    std::vector<double> v_cbluron(size);    
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);     
    corebluron::pack<mechanism::channel::Na,double,memory::AoSoA> v_cyme(1,0); // pack 1 channel
    
    init(v_cyme,v_cbluron);

    nrn_cur(&v_cbluron[0],1); // corebluron version
   
    typedef corebluron::pack<mechanism::channel::Na,double,memory::AoSoA>::iterator it_def;
    it_def it = v_cyme.begin(); 

    mechanism::channel::Na::cnrn_cur<it_def,memory::AoSoA>(it); 

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), 0.0001);
}

