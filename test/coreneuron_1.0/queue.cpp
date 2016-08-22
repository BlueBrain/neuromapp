#define BOOST_TEST_MODULE QueueTEST

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/array.hpp>

#include <numeric>
#include <iostream>
#include <algorithm>

#include "coreneuron_1.0/queue/tool/priority_queue.hpp"

//Test only MH, not std or boost, captain obvious


typedef boost::mpl::list<tool::sptq_queue<int,std::greater<int> >,
                         tool::sptq_queue<float,std::greater<float> >,
                         tool::sptq_queue<double,std::greater<double> >,
                         tool::bin_queue<int>,
                         tool::bin_queue<float>,
                         tool::bin_queue<double> > full_test_types;


BOOST_AUTO_TEST_CASE_TEMPLATE(constructor,T,full_test_types) {
    typedef T value_type;
    value_type queue;
    BOOST_CHECK_EQUAL(queue.size(),0.);
    BOOST_CHECK_EQUAL(queue.top(),0.);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(push_pop,T,full_test_types) {
    typedef T value_type;
    value_type queue;
    queue.push(1);
    BOOST_CHECK_EQUAL(queue.size(),1); // integer
    BOOST_CHECK_EQUAL(queue.top(),1); // correct because 1 has a exact IEEE representation
    queue.pop();
    BOOST_CHECK_EQUAL(queue.size(),0); // integer
}

BOOST_AUTO_TEST_CASE(push_pop_random_less) {
    tool::sptq_queue<int> q; // std::less by default
    boost::array<int,10> a;
    for(int i=0 ; i < 10; i++) // std::iota ...
	a[i] = i;
    std::random_shuffle(a.begin(), a.end());
    boost::array<int,10>::iterator it;
    it = a.begin();
    // fill the queue and sort
    while(it != a.end()){
        q.push(*it);
        it++;
    }

    int total(10);
    while(!q.empty()){
        int top = q.top();
        q.pop();
        BOOST_CHECK_EQUAL(top,--total); // carefull --total != total-- here
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(push_pop_random_greater,T,full_test_types) {
    typedef T value_type;
    typedef typename T::value_type nested_value_type;

    value_type queue;
    boost::array<nested_value_type,10> a;
    for(int i=0 ; i < 10; i++)
	a[i] = i;
    std::random_shuffle(a.begin(), a.end());
    typename boost::array<nested_value_type,10>::iterator it;
    it = a.begin();
    // fill the queue and sort
    while(it != a.end()){
        queue.push(*it);
        it++;
    }

    int total(0);
    while(!queue.empty()){
        int top = queue.top();
        queue.pop();
        BOOST_CHECK_EQUAL( top, total++);
    }

}

