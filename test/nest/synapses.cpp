/*
 * Neuromapp - args.cpp, Copyright (c), 2015,
 * Till Schumann - Swiss Federal Institute of technology in Lausanne,
 * till.schumann@epfl.ch,
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
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/nest/synapse.cpp
 *  Test on the nest synapse module
 */

#define BOOST_TEST_MODULE SynapseTest
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "utils/error.h"

#include "nest/synapse/models/tsodyks2.h"
#include "nest/synapse/connector_base.h"
#include "nest/synapse/connectionmanager.h"
#include "nest/synapse/event.h"
#include "nest/synapse/scheduler.h"
#include "nest/synapse/node.h"
#include "nest/synapse/drivers/synapse.h"

#include "coreneuron_1.0/common/data/helper.h" // common functionalities


using nest::ConnectorBase;
using nest::Connector;
using nest::vector_like;
using nest::tsodyks2;

BOOST_AUTO_TEST_CASE(nest_model_test)
{
    std::vector<std::string> command_v;
    int error(mapp::MAPP_OK);

    //no input
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("connection"); // dummy argument to be compliant with getopt
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_OK);

    //trying out wrong model
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("connection"); // dummy argument to be compliant with getopt
    command_v.push_back("--model");
    command_v.push_back("synnotthere"); // model does not exist
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //trying out wrong model parameters
	#ifdef _DEBUG //model parameters are only checked in debug mode
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("connection"); // dummy argument to be compliant with getopt
    command_v.push_back("--model");
    command_v.push_back("tsodyks2");
    command_v.push_back("--U");
    command_v.push_back("2.0"); // model does not exist
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);
	#endif //_DEBUG

    //trying out connector
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("connector"); // dummy argument to be compliant with getopt
    command_v.push_back("--fanout");
    command_v.push_back("2");
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_OK);

    //trying out invalid dt
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("connection"); // dummy argument to be compliant with getopt
    command_v.push_back("--nSpikes");
    command_v.push_back("0"); // model does not exist
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //asking for helper
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--help"); // help menu
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_USAGE);
}

BOOST_AUTO_TEST_CASE(nest_synapse_event_test)
{
    int iterations = 10;
    nest::spikedetector detector;
    std::vector<nest::spikeevent> events(iterations);
    for(unsigned int i = 0; i < iterations; ++i){
        nest::Time t(i*10.0);

        events[i].set_stamp(t);
        events[i].set_receiver(&detector);
        events[i]();

        /**Check that the previous event was enqueued correctly*/
        BOOST_REQUIRE_EQUAL(detector.spikes.size(), i+1);
        BOOST_REQUIRE_CLOSE(detector.spikes.back().get_stamp().get_ms(),t.get_ms(), 0.01);
    }
}

BOOST_AUTO_TEST_CASE(nest_synapse_vectorevent_test)
{
    std::vector<double> weights;

    const double weight = 1.;
    const double delay = 0.1;
    const double U = 0.5;
    const double u = 0.5;
    const double x = 1;
    const double tau_rec = 800;
    const double tau_fac = 0;
    const double dt = 0.1;
    int iterations = 10;
    
    nest::scheduler test_env;

    nest::spikedetector detector;
    nest::scheduler sch;
    nest::targetindex target = nest::scheduler::add_node(&detector);

    std::cout << "target=" << target << std::endl;

    nest::tsodyks2 syn(delay, weight, U, u, x, tau_rec, tau_fac, target);
    std::vector<nest::spikeevent> events(iterations);

    //trigger events - push weights to vector
    double t_last_spike = 0.0;
    for(size_t i = 0; i < events.size(); ++i){
        nest::Time t(i*10.0);
        events[i].set_stamp(t);
        syn.send(events[i], t_last_spike);
        t_last_spike += dt;

        BOOST_REQUIRE_EQUAL(detector.spikes.size(), i+1);
        BOOST_REQUIRE_CLOSE(detector.spikes.back().get_stamp().get_ms(),t.get_ms(), 0.01);
    }
}

BOOST_AUTO_TEST_CASE(nest_synapse_tsodyks2_constructor_test)
{
    //parameters
    const long d = 2;
    const double weight = 1.0;
    const double U = 0.5;
    const double u = 0.3;
    const double x = 1;
    const double tau_rec = 800.0;
    const double tau_fac = 20.0;

    nest::tsodyks2 synapse(d, weight, U, u, x, tau_rec, tau_fac, -1);

    BOOST_REQUIRE_CLOSE(synapse.U(), U, 0.01);
    BOOST_REQUIRE_CLOSE(synapse.u(), u, 0.01);
    BOOST_REQUIRE_CLOSE(synapse.x(), x, 0.01);
    BOOST_REQUIRE_EQUAL(synapse.delay(), d);
    BOOST_REQUIRE_CLOSE(synapse.weight(), weight, 0.01);
    BOOST_REQUIRE_CLOSE(synapse.tau_rec(), tau_rec, 0.01);
    BOOST_REQUIRE_CLOSE(synapse.tau_fac(), tau_fac, 0.01);

}

BOOST_AUTO_TEST_CASE(nest_synapse_tsodyks2_test)
{   /*
    *
     * tsodyks2 model documentation from NEST
     *
     * This synapse model implements synaptic short-term depression and short-term facilitation
     * according to [1] and [2]. It solves Eq (2) from [1] and modulates U according to eq. (2) of [2].
     * [1] Tsodyks, M. V., & Markram, H. (1997). The neural code between neocortical pyramidal neurons
     *  depends on neurotransmitter release probability. PNAS, 94(2), 719-23.
     * [2] Fuhrmann, G., Segev, I., Markram, H., & Tsodyks, M. V. (2002). Coding of temporal
     * information by activity-dependent synapses. Journal of neurophysiology, 87(1), 140-8.
     * [3] Maass, W., & Markram, H. (2002). Synapses as dynamic memory buffers. Neural networks, 15(2),
     * 155–61.synapse/args
    */

    std::vector<double> weights;
    
    nest::scheduler test_env;
    
    //parameters
    const long delay = 2;
    const double weight = 1.0;
    const double U = 0.5;
    const double u0 = 0.5;
    const double x0 = 1;
    const double tau_rec = 800.0;
    const double tau_fac = 20.0;
    const double dt = 1;

    nest::spikedetector detector;
    nest::targetindex target = nest::scheduler::add_node(&detector);

    nest::tsodyks2 synapse(delay, weight, U, u0, x0, tau_rec, tau_fac, target);
    //state variables
    double x = x0;
    double u = u0;
    for (int i=0; i<3; i++) {
        //generate spike
        nest::spikeevent se;
        se.set_stamp( dt*(i+1) ); // in Network::send< SpikeEvent >
        se.set_sender( NULL ); // in Network::send< SpikeEvent >
        se.set_weight(weight);
        se.set_delay(delay);

        synapse.send(se, dt*i);

        //solution from [3] equations (4) and (5):
        x = 1 + (x - x*u-1)*std::exp(-(dt/tau_rec));
        u = U + u *(1-U) * std::exp(-(dt/tau_fac));

        //solution from [3] equations (1):
        double w = x * u * weight;

        //check if parameters stay constant
        BOOST_REQUIRE_CLOSE(synapse.U(), U, 0.01);
        BOOST_REQUIRE_EQUAL(synapse.delay(), delay);
        BOOST_REQUIRE_CLOSE(synapse.weight(), weight, 0.01);
        BOOST_REQUIRE_CLOSE(synapse.tau_rec(), tau_rec, 0.01);
        BOOST_REQUIRE_CLOSE(synapse.tau_fac(), tau_fac, 0.01);

        //check variables
        BOOST_REQUIRE_CLOSE(synapse.x(), x, 0.01);
        BOOST_REQUIRE_CLOSE(synapse.u(), u, 0.01);

        //check results
        BOOST_REQUIRE_CLOSE(se.get_weight(), w, 0.01);
        BOOST_REQUIRE_EQUAL(se.get_delay(), delay);

        //check weight vector
        BOOST_REQUIRE_EQUAL(detector.spikes.size(), i+1);
        BOOST_REQUIRE_CLOSE(detector.spikes[i].get_weight(), w, 0.01);
    }
}

BOOST_AUTO_TEST_CASE(nest_grow_static_connector_test)
{
    nest::pool_env pevn;

    unsigned int K = K_CUTOFF-1;
    //create a connector with one tsodyks2 connection
    ConnectorBase* conn = NULL;

    for(unsigned int i = 1; i < K+1; ++i){
        tsodyks2 syn;
        conn = nest::add_connection< tsodyks2 >(conn, syn);
        BOOST_CHECK_EQUAL(conn->get_size(), i);
    }
}

/* When K >= K_CUTOFF (3), connector becomes a dynamic container.
 Behaviour should stay the same.
 */
BOOST_AUTO_TEST_CASE(nest_grow_dynamic_connector_test)
{
    nest::pool_env pevn;

    unsigned int K = K_CUTOFF+5;
    ConnectorBase* conn = NULL;
    for(unsigned int i = 1; i < K+1; ++i){
        tsodyks2 syn;
        conn = nest::add_connection<tsodyks2>(conn, syn);
        BOOST_CHECK_EQUAL(conn->get_size(), i);
    }
}

BOOST_AUTO_TEST_CASE(nest_connector_test){
    std::vector<std::string> command_v;
    int error(mapp::MAPP_OK);
    command_v.push_back("connector_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("connector"); // dummy argument to be compliant with getopt
    command_v.push_back("--nSpikes");
    command_v.push_back("3");
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_OK);
}

BOOST_AUTO_TEST_CASE(nest_connector_send) {
    nest::pool_env pevn;

    const double weight = 1.;
    const double delay = 0.1;
    const double U = 0.5;
    const double u = 0.5;
    const double x = 1;
    const double tau_rec = 800;
    const double tau_fac = 0;
    const double dt = 0.1;
    int iterations = 10;

    //nest::PoorMansAllocator poormansallocpool = nest::PoorMansAllocator();

    for (unsigned int k=1; k<K_CUTOFF+5; k++) {
        nest::scheduler test_env;
        
        std::vector<nest::spikedetector> detector(k);
        ConnectorBase* conn = NULL;

        for (unsigned int i=0; i<k; i++) {
            nest::tsodyks2 new_synapse(delay, weight, U, u, x, tau_rec, tau_fac, nest::scheduler::add_node(&(detector[i])));
            conn = nest::add_connection< tsodyks2 >(conn, new_synapse);
        }

        double x_i = x;
        double u_i = u;

        for (unsigned int i=0; i<5; i++) {
            nest::spikeevent se;
            se.set_stamp( dt*(i+1) );
            se.set_weight( weight );
            se.set_delay( delay );

            conn->send( se ); //thread removed (see synpase)

            //solution from [3] equations (4) and (5):
            x_i = 1 + (x_i - x_i*u_i-1)*std::exp(-(dt/tau_rec));
            u_i = U + u_i *(1-U) * std::exp(-(dt/tau_fac));

            //solution from [3] equations (1):
            const double w = x_i * u_i * weight;

            for (unsigned int j=0; j<k; j++) {
                BOOST_REQUIRE_EQUAL(detector[j].spikes.size(), i+1);
                BOOST_REQUIRE_CLOSE(detector[j].spikes[i].get_weight(), w, 0.01);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(nest_manager_) {
    nest::pool_env pevn;

    const int ncells = 2;

    namespace po = boost::program_options;
    po::variables_map vm;
    vm.insert(std::make_pair("nNeurons", po::variable_value(ncells, false)));
    vm.insert(std::make_pair("nThreads", po::variable_value(1, false)));

    vm.insert(std::make_pair("model", po::variable_value(std::string("tsodyks2"), false)));
    vm.insert(std::make_pair("delay", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("weight", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("U", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("u", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("x", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("tau_rec", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("tau_fac", po::variable_value(1.0, false)));

    nest::connectionmanager cm(vm);

    const int t=0;
    const int s_gid=1;

    nest::spikedetector sd;
    cm.connect(t, s_gid, nest::scheduler::add_node(&sd));
    cm.connect(t, s_gid, nest::scheduler::add_node(&sd));
    cm.connect(t, s_gid, nest::scheduler::add_node(&sd));

    nest::spikeevent se;
    cm.send(t, s_gid, se);

    BOOST_REQUIRE_EQUAL(sd.spikes.size(), 3);
}

BOOST_AUTO_TEST_CASE(nest_manager_build_from_neuron) {
    nest::scheduler test_env;

    nest::pool_env pevn;

    std::cout << "nest_manager_build_from_neuron" << std::endl;

    const int ncells = 50;
    const int outgoing = 20;
    namespace po = boost::program_options;

    po::variables_map vm;
    vm.insert(std::make_pair("nNeurons", po::variable_value(ncells, false)));
    vm.insert(std::make_pair("nThreads", po::variable_value(1, false)));

    vm.insert(std::make_pair("model", po::variable_value(std::string("tsodyks2"), false)));
    vm.insert(std::make_pair("delay", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("weight", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("U", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("u", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("x", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("tau_rec", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("tau_fac", po::variable_value(1.0, false)));

    //preallocate vector for results
    std::vector<nest::spikedetector> detectors(1);
    std::vector<nest::targetindex> detectors_targetindex(1);

    // register spike detectors

    for(unsigned int i=0; i < detectors.size(); ++i) {
        //scheduler stores pointers to the spike detectors
        detectors_targetindex[i] = nest::scheduler::add_node(&detectors[i]);  //add them to the scheduler
    }

    environment::continousdistribution neuro_dist(1, 0, ncells);
    environment::presyn_maker presyns(outgoing, environment::fixedoutdegree);
    presyns(0, &neuro_dist);

    nest::connectionmanager cm(vm);
    build_connections_from_neuron(0, neuro_dist, presyns, detectors_targetindex, cm);
    BOOST_REQUIRE_EQUAL(cm.connections_[ 0 ].size(), ncells);

    nest::spikeevent se;
    for (int i=0; i<ncells; i++) {
        //test number of outgoing connections
        BOOST_REQUIRE_EQUAL(cm.connections_[ 0 ].get(i)->get_size(), outgoing);
        //test event handling
        cm.send(0, i, se);
        BOOST_REQUIRE_EQUAL(detectors[0].spikes.size(), (i+1)*outgoing);
    }
}



