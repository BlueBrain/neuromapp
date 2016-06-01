/*
 * Neuromapp - args.cpp, Copyright (c), 2015,
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
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/nest/connector.cpp
 *  Test on the nest connector module
 */

#define BOOST_TEST_MODULE ConnectorTest
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "utils/error.h"

#include "nest/synapse/models/tsodyks2.h"
#include "nest/synapse/event.h"
#include "nest/synapse/drivers/synapse.h"
#include "nest/synapse/connector_base.h"
#include "nest/synapse/scheduler.h"

#include "coreneuron_1.0/common/data/helper.h" // common functionalities

using nest::ConnectorBase;
using nest::Connector;
using nest::vector_like;
using nest::tsodyks2;

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
