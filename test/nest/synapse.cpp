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
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_OK);

    //trying out wrong model
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--model");
    command_v.push_back("synnotthere"); // model does not exist
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //trying out wrong model parameters
	#ifdef _DEBUG //model parameters are only checked in debug mode
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--model");
    command_v.push_back("tsodyks2");
    command_v.push_back("--U");
    command_v.push_back("2.0"); // model does not exist
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);
	#endif //_DEBUG

    //trying out without connector
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--num_connections");
    command_v.push_back("2");
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //trying out connector
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--connector");
    command_v.push_back("--num_connections");
    command_v.push_back("2");
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_OK);

    //trying out invalid dt
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--dt");
    command_v.push_back("0"); // model does not exist
    error = mapp::execute(command_v,nest::model_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //trying out invalid iterations
    command_v.clear();
    command_v.push_back("model_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--iterations");
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
    unsigned int K = K_CUTOFF-1;
    //create a connector with one tsodyks2 connection
    ConnectorBase* conn = new Connector<1, tsodyks2>(tsodyks2());
    BOOST_CHECK_EQUAL(conn->get_size(), 1);
    for(unsigned int i = 2; i < K+1; ++i){
        conn = &((vector_like<tsodyks2>*)conn)->push_back(tsodyks2());
        BOOST_CHECK_EQUAL(conn->get_size(), i);
    }
}

/* When K >= K_CUTOFF (3), connector becomes a dynamic container.
 Behaviour should stay the same.
 */
BOOST_AUTO_TEST_CASE(nest_grow_dynamic_connector_test)
{
    unsigned int K = K_CUTOFF+5;
    ConnectorBase* conn = new Connector<1, tsodyks2>(tsodyks2());
    BOOST_CHECK_EQUAL(conn->get_size(), 1);
    for(unsigned int i = 2; i < K+1; ++i){
        conn = &((vector_like<tsodyks2>*)conn)->push_back(tsodyks2());
        BOOST_CHECK_EQUAL(conn->get_size(), i);
    }
}

BOOST_AUTO_TEST_CASE(nest_connector_test){
    std::vector<std::string> command_v;
    int error(mapp::MAPP_OK);
    command_v.push_back("connector_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--iterations");
    command_v.push_back("3");
    error = mapp::execute(command_v,nest::connector_execute);
    BOOST_CHECK(error==mapp::MAPP_OK);
}

BOOST_AUTO_TEST_CASE(nest_connector_send) {
    const double weight = 1.;
    const double delay = 0.1;
    const double U = 0.5;
    const double u = 0.5;
    const double x = 1;
    const double tau_rec = 800;
    const double tau_fac = 0;
    const double dt = 0.1;
    int iterations = 10;

    for (unsigned int k=1; k<K_CUTOFF+5; k++) {
        std::vector<nest::spikedetector> detector(k);
        nest::tsodyks2 synapse(delay, weight, U, u, x, tau_rec, tau_fac, nest::scheduler::add_node(&detector[0]));
        ConnectorBase* conn = new Connector<1, tsodyks2>(synapse);

        for (unsigned int i=1; i<k; i++) {
            nest::tsodyks2 new_synapse(delay, weight, U, u, x, tau_rec, tau_fac, nest::scheduler::add_node(&(detector[i])));
            conn = &((vector_like<tsodyks2>*)conn)->push_back(new_synapse);
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
        delete conn;
    }
}
