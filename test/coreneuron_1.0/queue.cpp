/*
 * Neuromapp - queue.cpp, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/coreneuron_1.0/queue.cpp
 *  Test on the queue miniapp
 */


#define BOOST_TEST_MODULE QueueTEST

#include <numeric>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/array.hpp>

#include "coreneuron_1.0/queue/queue.h"
#include "coreneuron_1.0/queue/tool/priority_queue.hpp"
#include "coreneuron_1.0/common/data/helper.h" // common functionalities
#include "utils/error.h"

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

BOOST_AUTO_TEST_CASE_TEMPLATE(remove_greater,T,full_test_types) {
    typedef T value_type;
    typedef typename T::value_type nested_value_type;
    typedef typename T::node_type node_type;


    value_type queue;
    boost::array<nested_value_type,10> a;
    for(int i=0 ; i < 10; i++)
        a[i] = i+2;
    std::random_shuffle(a.begin(), a.end());
    typename boost::array<nested_value_type,10>::iterator it;
    it = a.begin();
    // fill the queue and sort
    
    while(it != a.end()){
        queue.push(*it);
        it++;
    }
    
    node_type* n = new node_type(55);
    queue.push(n);
    BOOST_CHECK_EQUAL(queue.size(), 11);
    BOOST_CHECK_EQUAL(queue.top(), 2);
    tool::move(queue,n,1); //should 1 is the minimum value 
    BOOST_CHECK_EQUAL(queue.top() , 1); // check equal ok because 1 has a precise IEEE representation
    BOOST_CHECK_EQUAL(queue.size(), 11);
  }

BOOST_AUTO_TEST_CASE(helper_solver_test){
    std::vector<std::string> command_v;
    int error(mapp::MAPP_OK);

    //call help
    command_v.clear();
    command_v.push_back("coreneuron10_queue_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--help"); // dummy argument to be compliant with getopt
    error = mapp::execute(command_v,coreneuron10_queue_execute);
    BOOST_CHECK(error==mapp::MAPP_USAGE);

    //wrong argument
    command_v.clear();
    command_v.push_back("coreneuron10_queue_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--tqrhqrhqethqhba"); // this does not exist
    error = mapp::execute(command_v,coreneuron10_queue_execute);
    BOOST_CHECK(error==mapp::MAPP_UNKNOWN_ERROR);

    //wrong size
    command_v.clear();
    command_v.push_back("coreneuron10_queue_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--size"); // this does not exist
    command_v.push_back("-10"); // this does not exist
    error = mapp::execute(command_v,coreneuron10_queue_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_ARG);

    //wrong app
    command_v.clear();
    command_v.push_back("coreneuron10_queue_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--benchmark"); // this does not exist
    command_v.push_back("tim_app"); // this does not exist
    error = mapp::execute(command_v,coreneuron10_queue_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_ARG);

    //good app
    command_v.clear();
    command_v.push_back("coreneuron10_queue_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--benchmark"); // this does not exist
    command_v.push_back("pop"); // this does not exist
    command_v.push_back("--io"); // this does not exist
    command_v.push_back("true"); // this does not exist
    error = mapp::execute(command_v,coreneuron10_queue_execute);
    BOOST_CHECK(error==mapp::MAPP_OK);
    std::ifstream file("pop.csv");
    BOOST_CHECK((bool)file==true);


    command_v.clear();
    command_v.push_back("coreneuron10_queue_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--benchmark"); // this does not exist
    command_v.push_back("push_one"); // this does not exist
    command_v.push_back("--io"); // this does not exist
    command_v.push_back("false"); // this does not exist
    error = mapp::execute(command_v,coreneuron10_queue_execute);
    BOOST_CHECK(error==mapp::MAPP_OK);
    std::ifstream file2("push_one.csv");
    BOOST_CHECK((bool)file2==false);

}



