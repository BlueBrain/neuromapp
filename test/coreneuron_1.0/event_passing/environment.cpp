/*
 * Neuromapp - environment.cpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/coreneuron_1.0/event_passing/environment.cpp
 *  Test on the event passing environment.
 */

#define BOOST_TEST_MODULE enviroTest
#include <boost/test/unit_test.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>
#include <stdlib.h>
#include <time.h>
#include <ctime>

#include "coreneuron_1.0/event_passing/environment/environment.h"
#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

/**
 * Test the constructor of sim_constraints class
 */
BOOST_AUTO_TEST_CASE(constraints_constructor){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(0, 10);
    int ngroups = uniform(rng);
    int nprocs = uniform(rng);
    int rank = uniform(rng);
    int simtime = uniform(rng);
    sim_constraints sc(ngroups, nprocs, rank, simtime);

    BOOST_CHECK(sc.get_ngroups() == ngroups);
    BOOST_CHECK(sc.get_nprocs() == nprocs);
    BOOST_CHECK(sc.get_rank() == rank);
    BOOST_CHECK(sc.get_simtime() == simtime);
    BOOST_CHECK(sc.get_time() == 0);
}

/**
 * Test increment time for sim_constraints class
 */
BOOST_AUTO_TEST_CASE(constraints_increment_time){
    sim_constraints sc();
    sc.increment_time();
    BOOST_CHECK(sc.get_time() == 1.0);
}

/**
 * Test the constructor of event_generator class
 */
BOOST_AUTO_TEST_CASE(generator_constructor){
}

/**
 * Test operator() of event_generator class
 */
BOOST_AUTO_TEST_CASE(generator_functor_test){
}


/**
 * Test the constructor of presyn_maker class
 */
BOOST_AUTO_TEST_CASE(presyns_constructor){
}

/**
 * Test operator() of  class
 */
BOOST_AUTO_TEST_CASE(presyns_functor_test){
}

