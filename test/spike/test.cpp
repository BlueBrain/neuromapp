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
#include "spike/algos.hpp"
#include "spike/spike.h"
#include "spike/mpispikegraph.h"
#include "utils/error.h"

namespace bfs = ::boost::filesystem;

struct MPIInitializer {
	MPIInitializer(){
		MPI::Init();
	}
	~MPIInitializer(){
		MPI::Finalize();
	}
};

BOOST_GLOBAL_FIXTURE(MPIInitializer);

BOOST_AUTO_TEST_CASE(constructor_create_type){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();
    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = 2;
    size_t simtime = 10;

    MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, simtime);
}

BOOST_AUTO_TEST_CASE(graph_setup_test){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = rand()%(size*numOut);
    size_t simtime = 10;

    MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer,simtime);
    sg.setup();
    BOOST_CHECK(sg.input_presyns_.size() == numIn);
    BOOST_CHECK(sg.output_presyns_.size() == numOut);
}

BOOST_AUTO_TEST_CASE(distributed_setup_test){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = rand()%(size*numOut);
    size_t simtime = 10;

    DistributedSpikeGraph dsg(size,rank,numOut,numIn,eventsPer,simtime);
    dsg.setup();
    BOOST_CHECK(dsg.in_neighbors_.size() <= numIn);
    BOOST_CHECK(dsg.out_neighbors_.size() <= size);
    BOOST_CHECK(dsg.input_presyns_.size() == numIn);
    BOOST_CHECK(dsg.output_presyns_.size() == numOut);
}

BOOST_AUTO_TEST_CASE(distributed_one_inpresyn){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = 1;
    size_t simtime = 10;

    DistributedSpikeGraph dsg(size, rank, numOut, numIn, eventsPer, simtime);
    dsg.setup();
    //with one input presyn per node, in_neighbors.size() == input_presyns.size()
    BOOST_CHECK(dsg.in_neighbors_.size() == numIn);
}

BOOST_AUTO_TEST_CASE(graph_generate_spikes){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = rand()%(size*numOut);
    size_t simtime = 10;
    size_t mindelay = 5;

    MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, simtime);
    sg.setup();

    sg.load_send_buf();

    BOOST_CHECK(sg.send_buf_.size() == (simtime *mindelay));
    //check that the new spike's dest field is set to one of sg's output_presyns
    bool isValid = false;
    spike_item sitem;

    //check that each new spike matches one of sg's output presyns
    for(int i = 0; i < mindelay; ++i){
        sitem = sg.send_buf_.back();
        sg.send_buf_.pop_back();
        for(int i = 0; i < sg.output_presyns_.size(); ++i){
            if(sitem.dst_ == sg.output_presyns_[i]){
                isValid = true;
                break;
            }
        }
        BOOST_CHECK(isValid);
        isValid = false;
    }

}

BOOST_AUTO_TEST_CASE(graph_matches_test){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    size_t eventsPer = 10;
    size_t numOut = 2;
    size_t numIn = (rand()%(size*numOut-1)) + 1;
    //numIn is some value in the range [1, total number of output _presyns]
    size_t simtime = 10;
    size_t mindelay = 5;

    MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, simtime);
    sg.setup();

    //A spike I create should not have one of my presyns
    sg.load_send_buf();
    for(int i = 0; i < (eventsPer * mindelay); ++i){
        if(!sg.send_buf_.empty()){
            spike_item spike1 = sg.send_buf_.back();
            sg.send_buf_.pop_back();
            BOOST_CHECK(!sg.matches(spike1));
        }
        else
            break;
    }

    //Should match with a spike containing one of my input presyns
    int index = rand()%(sg.input_presyns_.size());
    spike_item spike2;
    spike2.t_ = 0.0;
    spike2.dst_ = sg.input_presyns_[index];
    BOOST_CHECK(sg.matches(spike2));
}
/*
BOOST_AUTO_TEST_CASE(algos_spike_exchange){
    int size = MPI::COMM_WORLD.Get_size();
    int rank = MPI::COMM_WORLD.Get_rank();

    srand(time(NULL) + rank);

    int eventsPer = 10;
    int numOut = 2;
    int numIn = rand()%(size*numOut);
    size_t simtime = 10;
    MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, simtime);
    sg.setup();
    sg.generate_spikes();

    spike_exchange(sg);

    BOOST_CHECK(sg.total_received_ == (eventsPer * size));

    DistributedSpikeGraph dsg(size, rank, numOut, numIn, eventsPer, simtime);
    dsg.setup();

    spike_exchange(dsg);

    BOOST_CHECK(dsg.recv_buf_.size() == (eventsPer * dsg.in_neighbors_.size()));
}*/
