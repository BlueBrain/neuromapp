/*
 * Neuromapp - test.cpp, Copyright (c), 2015,
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
 * @file neuromapp/test/spike/test.cpp
 *  Test on the Spike Miniapp.
 */

#define BOOST_TEST_MODULE SpikeTest
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <numeric>

#include "coreneuron_1.0/common/data/helper.h"
#include "coreneuron_1.0/queueing/pool.h"
#include "spike/algos.hpp"
#include "spike/spike.h"
#include "spike/environment.h"
#include "spike/spike.h"
#include "utils/error.h"
#include "test/spike/spike_test_header.hpp"
namespace bfs = ::boost::filesystem;

//Performs MPI init/finalize
#include "test/tools/mpi_helper.h"

/**
 * tests the create_spike_type() function.
 * checks that this data type can be freed using
 * MPI_Type_free
 */
BOOST_AUTO_TEST_CASE(create_spike_type_test){
    MPI_Datatype spike = create_spike_type();
    MPI_Type_free(&spike);
}

/**
 * tests the random_gen class's random number function
 *  with uniform distrubution, gen_uni().
 */
BOOST_AUTO_TEST_CASE(random_uniform_test){
    int seed = time(NULL);
    int max = 10;
    double lambda = 3.0;
    spike::random_gen rng(seed, max, lambda);
    int result = rng.gen_uni();
    BOOST_CHECK(result >= 0);
}

/**
 * tests the random_gen class's random number function
 *  with exponential distrubution, gen_exp().
 */
BOOST_AUTO_TEST_CASE(random_exponential_test){
    int seed = time(NULL);
    int max = 10;
    int lambda = 3;
    spike::random_gen rng(seed, max, lambda);
    double result = rng.gen_exp();
    BOOST_CHECK((result >= 0) && (result <= max));
}

/**
 * tests that the pool and environment class constructors
 * correctly initialize input/output presyns containers.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(setup_test, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    int nspikes = 10;
    int simtime = 10;
    int netcons = 5;
    int numOut = 2;
    int numIn = rand()%(numOut * (size - 1) - 1) + 1;

    T env(nspikes, simtime, numOut, numIn, netcons, size, rank);

    if(size == 1)
    	BOOST_CHECK(env.input_presyns_.size() == 0);
    else
    	BOOST_CHECK(env.input_presyns_.size() == numIn);

    BOOST_CHECK(env.output_presyns_.size() == numOut);
}

/**
 * for queueing::pool and spike::environment
 * test that generate_all_events() function
 * correctly initialize input/output presyns containers.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(generate_spikes, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    int nspikes = 10;
    int simtime = 10;
    int netcons = 5;
    int numOut = 2;
    int numIn = rand()%(numOut * (size - 1) - 1) + 1;

    T env(nspikes, simtime, numOut, numIn, netcons, size, rank);
    env.generate_all_events();

    env.fixed_step();

    //check that the new spike's dest field is set to one of env's output_presyns
    bool isValid = false;
    spike_item sitem;

    //check that each new spike matches one of env's output presyns
    while(!env.spikeout_.empty()){
        sitem = env.spikeout_.back();
        env.spikeout_.pop_back();
        for(int i = 0; i < env.output_presyns_.size(); ++i){
            if(sitem.data_ == env.output_presyns_[i]){
                isValid = true;
                break;
            }
        }
        BOOST_CHECK(isValid);
        isValid = false;
    }
}

/**
 * test that the set displ function works
 * set_displ iterates over the nin vector and outputs
 * it's displacements into the displ vector
 *
 * displ is the sum of nin (up to but not including current index)
 * for nin{1, 2, 3, 4} -> displ{0, 1, 3, 5}
 * for nin{6, 2, 5, 1} -> displ{0, 6, 8, 13}
 * etc.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(displ_test, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    int nspikes = 10;
    int simtime = 10;
    int netcons = 5;
    int numOut = 2;
    int numIn = rand()%(numOut * (size - 1) - 1) + 1;

    T env(nspikes, simtime, numOut, numIn, netcons, size, rank);

    int num = rand()%5;
    for(int i = 0; i < size; ++i){
        env.nin_[i] = (i*num);
    }

    env.set_displ();
    int sum = 0;
    for(int i = 0; i < size; ++i){
        BOOST_CHECK(env.displ_[i] == sum);
        sum += env.nin_[i];
    }
}

/**
 * for queueing::pool and spike::environment
 * test that run sim function results in the expected end state
 * for the case of blocking spike exchange (random # of input_presyns)
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(blocking_spike_exchange, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    int nspikes = 10;
    int simtime = 10;
    int netcons = 5;
    int numOut = 2;
    int numIn = rand()%(numOut * (size - 1) - 1) + 1;
    T env(nspikes, simtime, numOut, numIn, netcons, size, rank);
    run_sim(env, false);
    BOOST_CHECK(env.received() <= nspikes * size);
    std::cout<<env.received()<<" expected: "<<nspikes * size<<std::endl;
    std::cout<<"received" <<env.received()<<std::endl;
    std::cout<<"relevant" <<env.relevant()<<std::endl;
    BOOST_CHECK(env.received() > 0);
}

/**
 * for queueing::pool and spike::environment
 * test that run sim function results in the expected end state
 * for the case of blocking spike exchange (max # of input_presyns)
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(blocking_max_input_presyns, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    int nspikes = 10;
    int simtime = 10;
    int netcons = 5;
    int numOut = 4;
    int numIn = numOut*(size - 1);
    //simtime must be a multiple of min_delay (5) for this to pass
    T env(nspikes, simtime, numOut, numIn, netcons, size, rank);
    run_sim(env,false);
    BOOST_CHECK(env.relevant() >= (env.received() - nspikes));

}

//non-blocking is not supported on BGQ
#if MPI_VERSION >= 3 && 1 == 0
/**
 * for queueing::pool and spike::environment
 * test that run sim function results in the expected end state
 * for the case of nonblocking spike exchange (random # of input_presyns)
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(nonblocking_spike_exchange, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    int nspikes = 10;
    int simtime = 10;
    int netcons = 5;
    int numOut = 4;
    int numIn = rand()%(numOut * (size - 1) - 1) + 1;
    T env(nspikes, simtime, numOut, numIn, netcons, size, rank);
    run_sim(env,true);
    BOOST_CHECK(env.received() <= nspikes * size);
    BOOST_CHECK(env.received() > 0);
}

/**
 * for queueing::pool and spike::environment
 * test that run sim function results in the expected end state
 * for the case of nonblocking spike exchange (max # of input_presyns)
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(nonblocking_max_input_presyns, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    int nspikes = 10;
    int simtime = 10;
    int netcons = 5;
    int numOut = 4;
    int numIn = numOut*(size - 1);
    //simtime must be a multiple of min_delay (5) for this to pass
    T env(nspikes, simtime, numOut, numIn, netcons, size, rank);
    run_sim(env, true);
    BOOST_CHECK(env.relevant() >= (env.received() - nspikes));
}

#endif
