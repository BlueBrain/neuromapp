#include <regression/test_header.hpp>

#include "mechanism/channel/Na.hpp"
#include "mechanism/channel/NaTs2_t.hpp"
#include "mechanism/channel/SKv3_1.hpp"
#include "mechanism/channel/lm.hpp"
#include "mechanism/channel/lh.hpp"

using namespace cyme::test;

/* forward declaration */

int rates( double *_p);
int states( double *_p);

// c - __MECHANISM__CHANNEL_ is given by cmake

BOOST_AUTO_TEST_CASE_TEMPLATE(rates_test, T, test_types){

    int size = mechanism::channel::__MECHANISM__CHANNEL_::value_size;

    std::vector<double> v_cbluron(size);
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);
    corebluron::pack<mechanism::channel::__MECHANISM__CHANNEL_, typename T::value_type,T::order,T::s> v_cyme(1); // pack 1 channel

    init(v_cyme,v_cbluron);

    rates(&v_cbluron[0]); // corebluron version

    typedef typename corebluron::pack<mechanism::channel::__MECHANISM__CHANNEL_,typename T::value_type,T::order,T::s>::container_value container;
    typename container::iterator it = v_cyme.begin();
    mechanism::channel::__MECHANISM__CHANNEL_::cnrn_rates<container>(*it);

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(states_test, T, test_types){
    int size = mechanism::channel::__MECHANISM__CHANNEL_::value_size;

    std::vector<double> v_cbluron(size);
    std::generate(v_cbluron.begin(),v_cbluron.end(), GetRandom<double>);
    corebluron::pack<mechanism::channel::__MECHANISM__CHANNEL_, typename T::value_type,T::order,T::s> v_cyme(1); // pack 1 channel

    init(v_cyme,v_cbluron);

    states(&v_cbluron[0]); // corebluron version

    typedef typename corebluron::pack<mechanism::channel::__MECHANISM__CHANNEL_,typename T::value_type,T::order,T::s>::container_value container;
    typename container::iterator it = v_cyme.begin();
    mechanism::channel::__MECHANISM__CHANNEL_::cnrn_states<container>(*it);

    for(int i=0; i<size; ++i)
        BOOST_CHECK_CLOSE(v_cbluron[i],v_cyme(0,i), error_close<typename T::value_type>());
}
