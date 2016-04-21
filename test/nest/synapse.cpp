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
#include "nest/synapse/event.h"
#include "nest/synapse/synapse.h"

#include "coreneuron_1.0/common/data/helper.h" // common functionalities

BOOST_AUTO_TEST_CASE(nest_synapse_test){
	std::vector<std::string> command_v;
	int error(mapp::MAPP_OK);

	//no input
	command_v.push_back("synapse_execute"); // dummy argument to be compliant with getopt
	error = mapp::execute(command_v,nest::synapse_execute);
	BOOST_CHECK(error==mapp::MAPP_OK);

	//trying out wrong model parameter
	command_v.clear();
	command_v.push_back("synapse_execute"); // dummy argument to be compliant with getopt
	command_v.push_back("--model");
	command_v.push_back("synnotthere"); // model does not exist
	error = mapp::execute(command_v,nest::synapse_execute);
	BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

	//asking for helper
	command_v.clear();
	command_v.push_back("synapse_execute"); // dummy argument to be compliant with getopt
	command_v.push_back("--help"); // help menu
	error = mapp::execute(command_v,nest::synapse_execute);
	BOOST_CHECK(error==mapp::MAPP_USAGE);
}

BOOST_AUTO_TEST_CASE(nest_synapse_vectorevent_constructor_test){
	std::vector<double> weights;

	const double t = 0.1;
	const int sender = 1;
	const int receiver = 2;
	const double weight = 2.;
	const double delay = 0.5;

	nest::VectorEvent event1(t, sender, receiver, weight, delay, weights);

	BOOST_REQUIRE_EQUAL(event1.sender, sender);
	BOOST_REQUIRE_EQUAL(event1.receiver, receiver);
	BOOST_REQUIRE_CLOSE(event1.t, t, 0.001);
	BOOST_REQUIRE_CLOSE(event1.weight, weight, 0.001);
	BOOST_REQUIRE_CLOSE(event1.delay, delay, 0.001);
}

BOOST_AUTO_TEST_CASE(nest_synapse_vectorevent_test){

	std::vector<double> weights;

	const int sender = -1;
	const int receiver = -1;
	const double weight = 1.;
	const double delay = 0.1;

	nest::VectorEvent event1(1, sender, receiver, 0.1, delay, weights);
	nest::VectorEvent event2(2, sender, receiver, 0.2, delay, weights);
	nest::VectorEvent event3(3, sender, receiver, 0.3, delay, weights);

	//trigger events - push weights to vector
	event1();
	event2();
	event3();

	BOOST_REQUIRE_EQUAL(weights.size(), 3);
	BOOST_REQUIRE_CLOSE(weights[0], 0.1, 0.001);
	BOOST_REQUIRE_CLOSE(weights[1], 0.2, 0.001);
	BOOST_REQUIRE_CLOSE(weights[2], 0.3, 0.001);
}

BOOST_AUTO_TEST_CASE(nest_synapse_tsodyks2_constructor_test){
	//parameters
	const double delay = 0.2;
	const double weight = 1.0;
	const double U = 0.5;
	const double u = 0.3;
	const double x = 1;
	const double tau_rec = 800.0;
	const double tau_fac = 20.0;

	nest::Tsodyks2 synapse(delay, weight, U, u, x, tau_rec, tau_fac);

	BOOST_REQUIRE_CLOSE(synapse.get_U(), U, 0.01);
	BOOST_REQUIRE_CLOSE(synapse.get_u(), u, 0.01);
	BOOST_REQUIRE_CLOSE(synapse.get_x(), x, 0.01);
	BOOST_REQUIRE_CLOSE(synapse.get_delay(), delay, 0.01);
	BOOST_REQUIRE_CLOSE(synapse.get_weight(), weight, 0.01);
	BOOST_REQUIRE_CLOSE(synapse.get_tau_rec(), tau_rec, 0.01);
	BOOST_REQUIRE_CLOSE(synapse.get_tau_fac(), tau_fac, 0.01);

}

BOOST_AUTO_TEST_CASE(nest_synapse_tsodyks2_test){
	/**
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
	const double delay = 0.2;
	const double weight = 1.0;
	const double U = 0.5;
	const double u0 = 0.5;
	const double x0 = 1;
	const double tau_rec = 800.0;
	const double tau_fac = 20.0;
	const double dt = 1;

	nest::Tsodyks2 synapse(delay, weight, U, u0, x0, tau_rec, tau_fac);

	//state variables
	double x = x0;
	double u = u0;
	for (int i=0; i<3; i++) {
		//generate spike
		nest::VectorEvent spike(dt*(i+1), -1, -1, weight, delay, weights);

		synapse.send(spike, dt*i);

		//solution from [3] equations (4) and (5):
		x = 1 + (x - x*u-1)*std::exp(-(dt/tau_rec));
		u = U + u *(1-U) * std::exp(-(dt/tau_fac));

		//solution from [3] equations (1):
		double w = x * u * weight;

		//check if parameters stay constant
		BOOST_REQUIRE_CLOSE(synapse.get_U(), U, 0.01);
		BOOST_REQUIRE_CLOSE(synapse.get_delay(), delay, 0.01);
		BOOST_REQUIRE_CLOSE(synapse.get_weight(), weight, 0.01);
		BOOST_REQUIRE_CLOSE(synapse.get_tau_rec(), tau_rec, 0.01);
		BOOST_REQUIRE_CLOSE(synapse.get_tau_fac(), tau_fac, 0.01);

		//check variables
		BOOST_REQUIRE_CLOSE(synapse.get_x(), x, 0.01);
		BOOST_REQUIRE_CLOSE(synapse.get_u(), u, 0.01);

		//check results
		BOOST_REQUIRE_CLOSE(spike.weight, w, 1);
		BOOST_REQUIRE_CLOSE(spike.delay, delay, 1);

		//check weight vector
		BOOST_REQUIRE_EQUAL(weights.size(), i+1);
		BOOST_REQUIRE_CLOSE(weights[i], w, 1);
	}
}

