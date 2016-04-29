/*
 * Neuromapp - test.cpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/test/queuing/test.cpp
 *  Test on the Queueing Miniapp.
 */

#define BOOST_TEST_MODULE QueueingTest
#define IMPL T::impl

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "coreneuron_1.0/event_passing/queueing/queue.h"
#include "coreneuron_1.0/event_passing/queueing/pool.h"
#include "coreneuron_1.0/event_passing/queueing/thread.h"
#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"
#include "coreneuron_1.0/event_passing/spike/spike_interface.h"
#include "utils/error.h"
#include "utils/storage/neuromapp_data.h"
#include "coreneuron_1.0/common/data/helper.h"

namespace bfs = ::boost::filesystem;

//THREAD TESTS
/**
 * Unit test for nrn_thread_data::self_send function
 *
 *    - after n number of self-send events:
 *    	    pq_size == n
 *          enqueued_ == n
 */
BOOST_AUTO_TEST_CASE(thread_self_send){
    queueing::nrn_thread_data nt;
    int data = 0;
    double time = 0.0;
    int n = rand() % 10;

    for(int i = 0; i < n; ++i){
        nt.self_send(data,(time + i));
    }
    BOOST_CHECK(nt.pq_size() == n);
    BOOST_CHECK(nt.enqueued_ == n);
}

/**
 * Unit test for nrn_thread_data::inter_thread_send function
 *
 *    - after n number of inter_thread_send events:
 *          inter_thread_size == n
 *          inter_thread_received_ == n
 */
BOOST_AUTO_TEST_CASE(thread_inter_send){
    queueing::nrn_thread_data nt;
    int data = 0;
    double time = 0.0;
    int n = rand() % 10;

    for(int i = 0; i < n; ++i){
        nt.inter_thread_send(data, (time + i));
    }
    BOOST_CHECK(nt.inter_thread_size() == n);
    BOOST_CHECK(nt.ite_received_ == n);
}

/**
 * Unit test for nrn_thread_data::enqueue_my_events function
 *
 *    - checks the combination of enqueue and selfsend:
 *        self sends increases pq_size by n
 *        inter_thread_sends followed by an enqueue increases pq_size by m
 *    - pq_size = m + n
 *    - enqueued = m + n
 *    - inter_thread_events_ should be empty
 */
BOOST_AUTO_TEST_CASE(thread_enqueue){
    queueing::nrn_thread_data nt;
    int data = 0;
    double time = 0.0;
    int m = (rand() % 10) + 1;
    int n = (rand() % 10);

    nt.self_send(0.0,4.0);

    //inter_thread_sends/enqueue
    for(int i = 0; i < n; ++i){
            nt.inter_thread_send(data,(time + i));
    }
    nt.enqueue_my_events();

    //self_sends
    for(int i = 0; i < (m - 1); ++i){
            nt.self_send(data,(time + i));
    }

    BOOST_CHECK(nt.pq_size() == (m + n));
    BOOST_CHECK(nt.inter_thread_size() == 0);
    BOOST_CHECK(nt.enqueued_ == (m + n));
}

/*
 * Unit test for nrn_thread_data::deliver function
 *
 *     - verify that deliver function performs correctly
 *     (all events with time <= til are delivered)
 */
BOOST_AUTO_TEST_CASE(thread_deliver){
    queueing::nrn_thread_data nt;

	//enqueue 6 events
    nt.self_send(0,4.0);
    nt.inter_thread_send(0,1.0);
    nt.inter_thread_send(0,2.0);
    nt.inter_thread_send(0,3.0);
    nt.self_send(0,5.0);
    nt.enqueue_my_events();
    nt.self_send(0,6.0);
    BOOST_CHECK(nt.pq_size() == 6);


    //deliver the first item
    nt.increment_time();
    nt.deliver();
    BOOST_CHECK(nt.delivered_ == 1);
    BOOST_CHECK(nt.pq_size() == 5);

    //deliver the next 2
    nt.increment_time();
    nt.increment_time();
    while(nt.deliver())
        ;
    BOOST_CHECK(nt.delivered_ == 3);
    BOOST_CHECK(nt.pq_size() == 3);

    //deliver the remaining
    nt.increment_time();
    nt.increment_time();
    nt.increment_time();
    while(nt.deliver())
        ;
    BOOST_CHECK(nt.delivered_ == 6);
    BOOST_CHECK(nt.pq_size() == 0);
}

/**
 * Unit test for net_receive function
 *
 * Checks the validity of "coreneuron_1.0_net_receive_data after net_receive
 * using helper_check function (provided from coreneuron_1.0/common/data)
 */
BOOST_AUTO_TEST_CASE(net_receive){
    char name[] = "coreneuron_1.0_net_receive_data";
    std::string data = mapp::data_test();

    std::vector<char> chardata(data.begin(), data.end());
    chardata.push_back('\0');
    NrnThread* nt_ = (NrnThread*) storage_get(name, make_nrnthread, &chardata[0], free_nrnthread);
    if(nt_ == NULL){
        std::cout<<"Error: Unable to open data file"<<std::endl;
        storage_clear(name);
	exit(EXIT_FAILURE);
    }

    mech_net_receive(nt_, &(nt_->ml[18]));

    mapp::helper_check(name, "net_receive", mapp::data_test());
}


//POOL REGRESSION TESTING
/**
 * Tests the constructor of the pool function
 */
BOOST_AUTO_TEST_CASE(pool_constructor){
    int nprocs = 4;
    int ngroups = 8;
    int mindelay = 5;
    spike::spike_interface spike(nprocs);

    queueing::pool pl(false, ngroups, mindelay, 0, spike);
    BOOST_CHECK(pl.get_ngroups() == ngroups);
}

/**
 * Tests fixed_step function of the pool classi for one mindelay
 */
BOOST_AUTO_TEST_CASE(pool_fixed_step_1mindelay){
    int out = 4;
    int in = 4;
    int netconsper = 1;
    int ngroups = 8;
    int nspikes = 100;
    int rank = 0;
    int nprocs = 4;

    //only run one mindelay
    int mindelay = 5;
    int simtime = mindelay;

    //create the test environment
    environment::presyn_maker presyns(out, in, netconsper);
    spike::spike_interface spike(nprocs);

    //generate
    presyns(nprocs, ngroups, rank);
    environment::event_generator generator(nspikes, simtime, ngroups,
    rank, nprocs, out);
    int sum_events = 0;
    for(int i = 0; i < ngroups; ++i){
        sum_events += generator.get_size(i);
    }

    //process events
    queueing::pool pl(false, ngroups, mindelay, rank, spike);
    pl.fixed_step(generator, presyns);

    //check that every event went to the spikeout_ buffer
    BOOST_CHECK(sum_events == spike.spikeout_.size());
    std::cout<<"SUM: "<<sum_events<<" SPIKES: "<<spike.spikeout_.size()<<std::endl;
}

/**
 * Tests the fixed step function of the pool class for a larger simulation (100 dt)
 */
BOOST_AUTO_TEST_CASE(pool_send_ite){
    int out = 10;
    int in = 5;
    int netconsper = 1;
    int nprocs = 4;
    int ngroups = 8;
    int nspikes = 1000;
    int mindelay = 5;
    int simtime = 100;
    int rank = 0;

    //create the test environment
    environment::presyn_maker presyns(out, in, netconsper);
    spike::spike_interface spike(nprocs);

    //generate
    presyns(nprocs, ngroups, rank);
    environment::event_generator generator(nspikes, simtime,
    ngroups, rank, nprocs, out);

    int sum_events = 0;
    for(int i = 0; i < ngroups; ++i){
        sum_events += generator.get_size(i);
    }
    //process events
    queueing::pool pl(false, ngroups, mindelay, rank, spike);
    while(pl.get_time() <= simtime){
        pl.fixed_step(generator, presyns);
    }

    //check that every event went to the spikeout_ buffer
    BOOST_CHECK(spike.spikeout_.size() == sum_events);
}
