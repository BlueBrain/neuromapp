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

#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/event_generators.hpp"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

/**
 * Test the constructor of presyn_maker class
 */
BOOST_AUTO_TEST_CASE(presyns_constructor){
    environment::presyn_maker p1;

    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(1, 10);
    int ncells = uniform(rng);
    int fanin = ncells / 2 + 1;
    int netcons = uniform(rng);
    environment::presyn_maker p2(ncells, fanin);
    //previously had tests here
}

/**
 * Test operator() of presyn_maker class
 */
BOOST_AUTO_TEST_CASE(presyns_functor_test){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(50, 100);

    //passed into the find function, but not used
    int ncells = uniform(rng);
    int fanin = ncells;
    environment::presyn_maker p(ncells, fanin);
    int nprocs = 3;
    int ngroups = 3;
    int rank = 0;
    //create presyns
    p(nprocs, ngroups, rank);

    int cellsper = ncells / nprocs;

    //check for valid output presyns
    bool valid_input = true;
    bool  valid_output = true;
    for(int i = 0; i < cellsper; ++i){
        if(!p.find_output((rank + i))){
            valid_output = false;
            std::cerr<<"Error: rank "<<rank<<" could not find output: "<<rank + i<<std::endl;
            break;
        }
    }
    BOOST_CHECK(valid_output);
    //ensure NO invalid input presyns
    for(int i = 0; i < cellsper; ++i){
        if(p.find_input((rank + i))){
            valid_input = false;
            std::cerr<<"Error: rank "<<rank<<" found invalid input: "<<rank + i<<std::endl;
            break;
        }
    }
    BOOST_CHECK(valid_input);
}

/**
 * For a graph with max number of input presyns, test that
 * all gid's that are not in the range [rank, rank + num out) are input presyns.
 * Check also that these are not output presyns gid's.
 */
BOOST_AUTO_TEST_CASE(presyns_find_test){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(50, 100);
    int ncells = uniform(rng);
    int nprocs = 3;
    int ngroups = 3;
    int rank = 0;
    int fanin = ncells;
    environment::presyn_maker p(ncells, fanin);
    p(nprocs, ngroups, rank);

    const int offset = ncells % nprocs;
    const bool hasonemore = offset > rank;
    int cellsper = ncells / nprocs + hasonemore;


    bool valid_input = true;
    bool valid_output = true;

    //check for valid input presyns
    for(int i = cellsper; i < ncells; ++i){
        if(!p.find_input(i)){
            std::cerr<<"Error: rank "<<rank<<" could not find input: "<<i<<std::endl;
            valid_input = false;
            break;
        }
        else if(p.find_output(i)){
            std::cerr<<"Error: rank "<<rank<<" found invalid output presyn: "<<i<<std::endl;
            valid_output = false;
            break;
        }
    }
    BOOST_CHECK(valid_input);
    BOOST_CHECK(valid_output);
}

/**
 * Test generation of events for kai_generator
 */
BOOST_AUTO_TEST_CASE(generator_kai){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(50, 100);
    int nspike = uniform(rng);
    int ncells = 10;
    int nprocs = 1;
    int ngroups = 1;
    int simtime = 100;
    int rank = 0;
    int fanin = ncells;

    //generate events
    environment::presyn_maker p(ncells, fanin);
    p(nprocs, ngroups, rank);

    environment::event_generator generator(ngroups);

    double mean = static_cast<double>(simtime) / static_cast<double>(nspike);
    double lambda = 1.0 / static_cast<double>(mean * ncells);

    environment::generate_events_kai(generator.begin(),
                            simtime, ngroups, rank, nprocs, ncells, lambda);

    environment::gen_event ev;
    BOOST_CHECK(!generator.empty(0));

    //check that all generated events have valid time and presyns
    bool valid_time = true;
    bool valid_gid = true;
    int gid = 0;
    while(!generator.empty(0)){
        ev = generator.pop(0);
        //time
        if(ev.second > simtime || ev.second < 0){
            valid_time = false;
            std::cerr<<"Error: invalid time: "<<ev.second<<std::endl;
            break;
        }
        gid = ev.first;
        if(!p.find_output(gid)){
            std::cerr<<"Error: gid not found: "<<gid<<std::endl;
            valid_gid = false;
            break;
        }
    }
    BOOST_CHECK(valid_time);
    BOOST_CHECK(valid_gid);
}

BOOST_AUTO_TEST_CASE(generator_poisson){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(50, 100);
    int nspike = uniform(rng);
    int ncells = 10;
    int nprocs = 1;
    int ngroups = 1;
    int simtime = 100;
    int rank = 0;
    int fanin = ncells;

    //generate events
    environment::presyn_maker p(ncells, fanin);
    p(nprocs, ngroups, rank);

    environment::event_generator generator(ngroups);

    double mean = static_cast<double>(simtime) / static_cast<double>(nspike);
    double lambda = 1.0 / static_cast<double>(mean * ncells);

    environment::generate_poisson_events(generator.begin(),
                            simtime, ngroups, rank, nprocs, ncells, lambda);

    environment::gen_event ev;
    BOOST_CHECK(!generator.empty(0));

    //check that all generated events have valid time and presyns
    bool valid_time = true;
    bool valid_gid = true;
    int gid = 0;
    while(!generator.empty(0)){
        ev = generator.pop(0);
        //time
        if(ev.second > simtime || ev.second < 0){
            valid_time = false;
            std::cerr<<"Error: invalid time: "<<ev.second<<std::endl;
            break;
        }
        gid = ev.first;
        if(!p.find_output(gid)){
            std::cerr<<"Error: gid not found: "<<gid<<std::endl;
            valid_gid = false;
            break;
        }
    }
    BOOST_CHECK(valid_time);
    BOOST_CHECK(valid_gid);
}

BOOST_AUTO_TEST_CASE(generator_uniform){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(50, 100);
    int nspike = uniform(rng);
    int ncells = 10;
    int nprocs = 1;
    int ngroups = 1;
    int simtime = 100;
    int rank = 0;
    int fanin = ncells;

    //generate events
    environment::presyn_maker p(ncells, fanin);
    p(nprocs, ngroups, rank);

    environment::event_generator generator(ngroups);

    double firing_freq = static_cast<double>(nspike) / static_cast<double>(simtime*ncells);
    int firing_interval = static_cast<int>(1.0 / firing_freq);

    environment::generate_poisson_events(generator.begin(),
                            simtime, ngroups, rank, nprocs, ncells, firing_interval);

    environment::gen_event ev;
    BOOST_CHECK(!generator.empty(0));

    //check that all generated events have valid time and presyns
    bool valid_time = true;
    bool valid_gid = true;
    int gid = 0;
    while(!generator.empty(0)){
        ev = generator.pop(0);
        //time
        if(ev.second > simtime || ev.second < 0){
            valid_time = false;
            std::cerr<<"Error: invalid time: "<<ev.second<<std::endl;
            break;
        }
        gid = ev.first;
        if(!p.find_output(gid)){
            std::cerr<<"Error: gid not found: "<<gid<<std::endl;
            valid_gid = false;
            break;
        }
    }
    BOOST_CHECK(valid_time);
    BOOST_CHECK(valid_gid);
}


/**
 * Test the compare less than/equals function of event_generator class
 */
BOOST_AUTO_TEST_CASE(generator_compare_top_lte){
    int nspike = 100;
    int ncells = 10;
    int nprocs = 1;
    int ngroups = 5;
    int simtime = 100;
    int rank = 0;

    //generate events
    environment::event_generator generator(ngroups);

    double mean = static_cast<double>(simtime) / static_cast<double>(nspike);
    double lambda = 1.0 / static_cast<double>(mean * nprocs);

    environment::generate_events_kai(generator.begin(),
                             simtime, ngroups, rank, nprocs, ncells, lambda);

    bool greater_than_min = true;
    bool less_than_max = true;
    for(int i = 0; i < ngroups; ++i){
        //all events should have a time greater than -1.0
        if(generator.compare_top_lte(i, -1.0)){
            greater_than_min = false;
            environment::gen_event g = generator.pop(i);
            std::cerr<<"Event time: "<<g.first<<" =< -1.0"<<std::endl;
            break;
        }
        //all events should have a time less than simtime x 2
        if(!generator.compare_top_lte(i, 200.0)){
            less_than_max = false;
            environment::gen_event g = generator.pop(i);
            std::cerr<<"Event time: "<<g.first<<" > 200"<<std::endl;
            break;
        }
    }
    BOOST_CHECK(greater_than_min);
    BOOST_CHECK(less_than_max);
}
