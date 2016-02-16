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
#include "utils/error.h"
#include "test/spike/spike_test_header.hpp"
namespace bfs = ::boost::filesystem;

struct MPIInitializer {
	MPIInitializer(){
		MPI::Init();
	}
	~MPIInitializer(){
		MPI::Finalize();
	}
};

//performs mpi initialization/finalize
BOOST_GLOBAL_FIXTURE(MPIInitializer);

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
 * tests that the pool and environment class constructors
 * correctly initialize input/output presyns containers.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(setup_test, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = rand()%(numOut * (size - 1) - 1) + 1;

    T env(eventsPer, numOut, numIn, size, rank);

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

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = rand()%(numOut * (size - 1) - 1) + 1;
    size_t simtime = 10;

    T env(eventsPer, numOut, numIn, size, rank);
    env.generate_all_events(simtime);
    env.time_step();

    BOOST_CHECK(env.spikeout_.size() == (eventsPer * env.cells()));
    //check that the new spike's dest field is set to one of env's output_presyns
    bool isValid = false;
    spike_item sitem;

    //check that each new spike matches one of env's output presyns
    for(int i = 0; i < env.mindelay(); ++i){
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

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = rand()%(numOut * (size - 1) - 1) + 1;

    T env(eventsPer, numOut, numIn, size, rank);

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
 * test that matches() function returns false when spike destination
 * doesn not match any input_presyn (for example when it matches an output
 * presyn instead). Also tests that matchse returns true when spike
 * destination does match.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(env_matches_test, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = rand()%(numOut * (size - 1) - 1) + 1;
    //numIn is some value in the range [1, total number of output _presyns]
    size_t simtime = 10;

    T env(eventsPer, numOut, numIn, size, rank);
    env.generate_all_events(simtime);

    //A spike I create should not have one of my presyns
    env.time_step();

    spike_item spike1;
    while(!env.spikeout_.empty()){
        spike1 = env.spikeout_.back();
        env.spikeout_.pop_back();
        BOOST_CHECK(!env.matches(spike1));
    }
    //Should match with a spike containing one of my input presyns
    if(size > 1){
        int index = rand()%(env.input_presyns_.size());
        spike_item spike2;
        spike2.t_ = 0.0;
        spike2.data_ = env.input_presyns_[index];
        BOOST_CHECK(env.matches(spike2));
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

    int eventsPer = 10;
    int numOut = 2;
    size_t numIn = rand()%(numOut * (size - 1) - 1) + 1;
    size_t simtime = 10;
    T env(eventsPer, numOut, numIn, size, rank);
    run_sim(env,simtime,false);
    BOOST_CHECK(env.received() == (eventsPer * size * simtime *env.cells()));
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

    int eventsPer = 1;
    int numOut = 4;
    size_t numIn = numOut*(size - 1);
    //simtime must be a multiple of min_delay (5) for this to pass
    size_t simtime = 10;
    T env(eventsPer, numOut, numIn, size, rank);
    run_sim(env,simtime,false);
    BOOST_CHECK((env.received() - (eventsPer * simtime * env.cells())) == env.relevent());

}

//non-blocking is not supported on BGQ
#ifndef _ARCH_QP
/**
 * for queueing::pool and spike::environment
 * test that run sim function results in the expected end state
 * for the case of nonblocking spike exchange (random # of input_presyns)
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(nonblocking_spike_exchange, T, full_test_types){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    int eventsPer = 1;
    int numOut = 4;
    size_t numIn = rand()%(numOut * (size - 1) - 1) + 1;
    size_t simtime = 10;
    T env(eventsPer, numOut, numIn, size, rank);
    run_sim(env,simtime,true);
    BOOST_CHECK(env.received() == (eventsPer * size * simtime * env.cells()));
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

    int eventsPer = 10;
    int numOut = 4;
    size_t numIn = numOut*(size - 1);
    //simtime must be a multiple of min_delay (5) for this to pass
    size_t simtime = 10;
    T env(eventsPer, numOut, numIn, size, rank);
    run_sim(env,simtime,true);
    BOOST_CHECK((env.received() - (eventsPer*simtime*env.cells())) == env.relevent() );
}
#endif


/*
//TODO: implement distributed graph spike_exchange
BOOST_AUTO_TEST_CASE(distributed_setup_test){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = rand()%(size*numOut);
    size_t simtime = 10;

    DistributedSpikeGraph denv(size,rank,numOut,numIn,eventsPer,simtime);
    denv.setup();
    BOOST_CHECK(denv.in_neighbors_.size() <= numIn);
    BOOST_CHECK(denv.out_neighbors_.size() <= size);
    BOOST_CHECK(denv.input_presyns_.size() == numIn);
    BOOST_CHECK(denv.output_presyns_.size() == numOut);
}

BOOST_AUTO_TEST_CASE(distributed_one_inpresyn){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = 1;
    size_t simtime = 10;

    DistributedSpikeGraph denv(size, rank, numOut, numIn, eventsPer, simtime);
    denv.setup();
    //with one input presyn per node, in_neighbors.size() == input_presyns.size()
    BOOST_CHECK(denv.in_neighbors_.size() == numIn);
}
*/
