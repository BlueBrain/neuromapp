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
 * @file neuromapp/test/queuing/test.cpp
 *  Test on the Queueing Miniapp.
 */

#define BOOST_TEST_MODULE QueueingTest
#define IMPL T::impl

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "coreneuron_1.0/queueing/queueing.h"
#include "coreneuron_1.0/queueing/pool.h"
#include "coreneuron_1.0/queueing/thread.h"
#include "utils/error.h"
#include "utils/storage/neuromapp_data.h"
#include "coreneuron_1.0/common/data/helper.h"
#include "test/coreneuron_1.0/test_header.hpp"

namespace bfs = ::boost::filesystem;

//VALIDITY TESTS
BOOST_AUTO_TEST_CASE(mutex_test){
    char arg1[]="NULL";
    char arg2[]="--numthread=8";
    char arg3[]="--eventsper=25";
    char arg4[]="--simtime=50";
    char arg5[]="--percent-ite=90";
    char * const argv[] = {arg1, arg2, arg3, arg4, arg5};
    int argc = 5;
    BOOST_CHECK(queueing_execute(argc,argv)==0);
    neuromapp_data.clear("inter_received");
    neuromapp_data.clear("enqueued");
    neuromapp_data.clear("delivered");
    neuromapp_data.clear("spikes");
}

BOOST_AUTO_TEST_CASE(spinlock_test){
    char arg1[]="NULL";
    char arg2[]="--numthread=8";
    char arg3[]="--eventsper=25";
    char arg4[]="--simtime=50";
    char arg5[]="--percent-ite=90";
    char arg6[]="--spinlock";
    char * const argv[] = {arg1, arg2, arg3, arg4, arg5, arg6};
    int argc = 6;
    BOOST_CHECK(queueing_execute(argc,argv)==0);
    neuromapp_data.clear("inter_received");
    neuromapp_data.clear("enqueued");
    neuromapp_data.clear("delivered");
    neuromapp_data.clear("spikes");
}

BOOST_AUTO_TEST_CASE(full_ite){
    char arg1[]="NULL";
    char arg2[]="--numthread=8";
    char arg3[]="--eventsper=25";
    char arg4[]="--simtime=25";
    char arg5[]="--percent-ite=100";
    char * const argv[] = {arg1, arg2, arg3, arg4, arg5, arg5};
    int argc = 5;
    BOOST_CHECK(queueing_execute(argc,argv)==0);
    std::string key1("inter_received");
    BOOST_CHECK(neuromapp_data.has<int>(key1));
    std::string key2("enqueued");
    BOOST_CHECK(neuromapp_data.has<int>(key2));
    std::string key3("spikes");
    BOOST_CHECK(neuromapp_data.has<int>(key3));

    const int simtime = 25;
    const int cellgroups = 64;
    const int eventsper = 25;
    //verify the correct number of inter-thread events were received
    BOOST_CHECK(neuromapp_data.get<int>(key1) <= (simtime * cellgroups * eventsper));
    BOOST_CHECK(neuromapp_data.get<int>(key2) <= (simtime * cellgroups * eventsper));
    BOOST_CHECK(neuromapp_data.get<int>(key3) == 0);
    neuromapp_data.clear("inter_received");
    neuromapp_data.clear("enqueued");
    neuromapp_data.clear("delivered");
    neuromapp_data.clear("spikes");
}

BOOST_AUTO_TEST_CASE(full_ite_spinlock){
    char arg1[]="NULL";
    char arg2[]="--numthread=8";
    char arg3[]="--eventsper=25";
    char arg4[]="--simtime=25";
    char arg5[]="--percent-ite=100";
    char arg6[]="--spinlock";
    char * const argv[] = {arg1, arg2, arg3, arg4, arg5, arg6};
    int argc = 6;
    BOOST_CHECK(queueing_execute(argc,argv)==0);
    std::string key1("inter_received");
    BOOST_CHECK(neuromapp_data.has<int>(key1));
    std::string key2("enqueued");
    BOOST_CHECK(neuromapp_data.has<int>(key2));
    std::string key3("spikes");
    BOOST_CHECK(neuromapp_data.has<int>(key3));

    const int simtime = 25;
    const int cellgroups = 64;
    const int eventsper = 25;
    //verify the correct number of inter-thread events were received
    BOOST_CHECK(neuromapp_data.get<int>(key1) <= (simtime * cellgroups * eventsper));
    BOOST_CHECK(neuromapp_data.get<int>(key2) <= (simtime * cellgroups * eventsper));
    BOOST_CHECK(neuromapp_data.get<int>(key3) == 0);
    neuromapp_data.clear("inter_received");
    neuromapp_data.clear("enqueued");
    neuromapp_data.clear("delivered");
    neuromapp_data.clear("spikes");
}

BOOST_AUTO_TEST_CASE(no_ite){
    char arg1[]="NULL";
    char arg2[]="--numthread=8";
    char arg3[]="--eventsper=25";
    char arg4[]="--simtime=25";
    char arg5[]="--percent-ite=0";
    char * const argv[] = {arg1, arg2, arg3, arg4, arg5};
    int argc = 5;
    BOOST_CHECK(queueing_execute(argc,argv)==0);

    std::string key1("inter_received");
    BOOST_CHECK(neuromapp_data.has<int>(key1));
    std::string key2("enqueued");
    BOOST_CHECK(neuromapp_data.has<int>(key2));

    const int simtime = 25;
    const int cellgroups = 64;
    const int eventsper = 25;
    //verify that no inter-thread events were received
    BOOST_CHECK( neuromapp_data.get<int>(key1) == 0);
    //verify that the correct number of events were enqueued
    BOOST_CHECK(neuromapp_data.get<int>(key2) == (simtime * cellgroups * eventsper));
    neuromapp_data.clear("inter_received");
    neuromapp_data.clear("enqueued");
    neuromapp_data.clear("delivered");
    neuromapp_data.clear("spikes");
}

BOOST_AUTO_TEST_CASE(no_ite_spinlock){
    char arg1[]="NULL";
    char arg2[]="--numthread=8";
    char arg3[]="--eventsper=25";
    char arg4[]="--simtime=25";
    char arg5[]="--percent-ite=0";
    char arg6[]="--spinlock";
    char * const argv[] = {arg1, arg2, arg3, arg4, arg5, arg6};
    int argc = 6;
    BOOST_CHECK(queueing_execute(argc,argv)==0);

    std::string key1("inter_received");
    BOOST_CHECK(neuromapp_data.has<int>(key1));
    std::string key2("enqueued");
    BOOST_CHECK(neuromapp_data.has<int>(key2));

    const int simtime = 25;
    const int cellgroups = 64;
    const int eventsper = 25;
    //verify that no inter-thread events were received
    BOOST_CHECK( neuromapp_data.get<int>(key1) == 0);
    //verify that the correct number of events were enqueued
    BOOST_CHECK(neuromapp_data.get<int>(key2) == (simtime * cellgroups * eventsper));
    neuromapp_data.clear("inter_received");
    neuromapp_data.clear("enqueued");
    neuromapp_data.clear("delivered");
    neuromapp_data.clear("spikes");
}

BOOST_AUTO_TEST_CASE(spike_enabled){
    char arg1[]="NULL";
    char arg2[]="--numthread=8";
    char arg3[]="--eventsper=25";
    char arg4[]="--simtime=25";
    char arg5[]="--percent-ite=0";
    char arg6[]="--spike-enabled";
    char * const argv[] = {arg1, arg2, arg3, arg4, arg5, arg6};
    int argc = 6;
    BOOST_CHECK(queueing_execute(argc,argv)==0);

    std::string key1("spikes");
    BOOST_CHECK(neuromapp_data.has<int>(key1));

    const int simtime = 25;
    const int cellgroups = 64;
    const int eventsper = 25;
    const int percentspike = 3;
    //verify that the correct number of spikes were sent
    BOOST_CHECK(neuromapp_data.get<int>(key1) ==
		    (simtime * cellgroups * eventsper * percentspike / 100));
    neuromapp_data.clear("inter_received");
    neuromapp_data.clear("enqueued");
    neuromapp_data.clear("delivered");
    neuromapp_data.clear("spikes");
}

//UNIT TESTS
BOOST_AUTO_TEST_CASE_TEMPLATE(pool_choose_dest, T, full_test_types){
	queueing::Pool<IMPL> pl1(false, 20, 0, 0);
	int dst = 0;
	for(int i = 0; i < 10; ++i){
		dst = pl1.chooseDst(0);
		BOOST_CHECK(dst == 0);
	}

	queueing::Pool<IMPL> pl2(false, 20, 100, 0);
	for(int i = 0; i < 10; ++i){
		dst = pl2.chooseDst(0);
		BOOST_CHECK(dst != 0);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(thread_self_send, T, full_test_types){
	queueing::NrnThreadData<IMPL> nt;
	nt.selfSend(0.0,1.0);
	nt.selfSend(0.0,2.0);
	nt.selfSend(0.0,3.0);
	nt.selfSend(0.0,4.0);
	BOOST_CHECK(nt.PQSize() == 4);
	BOOST_CHECK(nt.enqueued_ == 4);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(thread_inter_send, T, full_test_types){
	queueing::NrnThreadData<IMPL> nt;
	nt.interThreadSend(0.0,1.0);
	nt.interThreadSend(0.0,2.0);
	nt.interThreadSend(0.0,3.0);
	nt.interThreadSend(0.0,4.0);
	BOOST_CHECK(nt.interThreadSize() == 4);
	BOOST_CHECK(nt.ite_received_ == 4);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(thread_enqueue, T, full_test_types){
	queueing::NrnThreadData<IMPL> nt;
	nt.selfSend(0.0,4.0);
	nt.interThreadSend(0.0,1.0);
	nt.interThreadSend(0.0,2.0);
	nt.interThreadSend(0.0,3.0);
	nt.enqueueMyEvents();
	nt.selfSend(0.0,5.0);
	BOOST_CHECK(nt.PQSize() == 5);
	BOOST_CHECK(nt.interThreadSize() == 0);
	BOOST_CHECK(nt.enqueued_ == 5);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(thread_deliver, T, full_test_types){
    queueing::NrnThreadData<IMPL> nt;
    nt.selfSend(0.0,4.0);
    nt.interThreadSend(0.0,1.0);
    nt.interThreadSend(0.0,2.0);
    nt.interThreadSend(0.0,3.0);
    nt.selfSend(0.0,5.0);
    nt.enqueueMyEvents();
	nt.selfSend(0.0,6.0);
    BOOST_CHECK(nt.PQSize() == 6);

	//deliver the first item
	nt.deliver(0,6);
    BOOST_CHECK(nt.delivered_ == 1);
    BOOST_CHECK(nt.PQSize() == 5);

	//deliver the next 2
	while(nt.deliver(0,3))
;
    BOOST_CHECK(nt.delivered_ == 3);
    BOOST_CHECK(nt.PQSize() == 3);
	std::cout<<"Delivered "<<nt.delivered_<<std::endl;
	std::cout<<"PQSize "<<nt.PQSize()<<std::endl;

	//deliver the remaining
	while(nt.deliver(0,6))
;
    BOOST_CHECK(nt.delivered_ == 6);
    BOOST_CHECK(nt.PQSize() == 0);
}
