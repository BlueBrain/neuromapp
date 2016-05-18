/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/nest/synapse/main.cpp
 * \brief NEST synapse Miniapp
 */

#include <iostream>
#include <string>


#include <boost/program_options.hpp>
#include <boost/chrono.hpp>
#include <boost/scoped_ptr.hpp>

#include "nest/synapse/synapse.h"
#include "nest/synapse/event.h"
#include "nest/synapse/models/tsodyks2.h"
#include "utils/error.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;

namespace nest
{
	/** \fn help(int argc, char *const argv[], po::variables_map& vm)
		\brief Helper using boost program option to facilitate the command line manipulation
		\param argc number of argument from the command line
		\param argv the command line from the driver or external call
		\param vm encapsulate the command line
		\return error message from mapp::mapp_error
	 */
	int synapse_help(int argc, char* const argv[], po::variables_map& vm)
	{
		po::options_description desc("Allowed options");
		desc.add_options()
		("help", "produce help message")
		("models", "list available synapse models")
		("model", po::value<std::string>()->default_value("tsodyks2"), "synapse model")

		// tsodyks2 parameters
		// synapse parameters are not checked
		("delay", po::value<double>()->default_value(1.0), "delay")
		("weight", po::value<double>()->default_value(1.0), "weight")
		("U", po::value<double>()->default_value(0.5), "U")
		("u", po::value<double>()->default_value(0.5), "u")
		("x", po::value<double>()->default_value(1), "x")
		("tau_rec", po::value<double>()->default_value(800.0), "tau_rec")
		("tau_fac", po::value<double>()->default_value(0.0), "tau_fac")


		// simulation parameters
		("dt", po::value<double>()->default_value(0.1), "time between spikes")
		("iterations", po::value<int>()->default_value(1), "number of iterations (spikes)");


		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		//check for valid synapse model & parameters
		if (vm["model"].as<std::string>() == "tsodyks2") {
			const double delay = vm["delay"].as<double>();
			const double weight = vm["weight"].as<double>();
			const double U = vm["U"].as<double>();
			const double u = vm["u"].as<double>();
			const double x = vm["x"].as<double>();
			const double tau_rec = vm["tau_rec"].as<double>();
			const double tau_fac = vm["tau_fac"].as<double>();

			try {
				spikedetector sd;
				tsodyks2 syn(delay, weight, U, u, x, tau_rec, tau_fac, &sd);
			}
			catch (std::invalid_argument& e) {
				std::cout << "Error in model parameters: " << e.what() << std::endl;
				return mapp::MAPP_BAD_DATA;
			}
		}
		/* else if ( more models ) */
		else {
			std::cout << "Error: Selected synapse model is  unknown" << std::endl;
						return mapp::MAPP_BAD_DATA;
		}

		//check for valid dt
		if (vm["dt"].as<double>() < 0.1) {
				std::cout << "Error: Time between spikes has to be bigger than 0.1" << std::endl;
				return mapp::MAPP_BAD_DATA;
		}

		//check for valid iterations
		if (vm["iterations"].as<int>() < 1) {
				std::cout << "Error: Number of iterations has to be a greater than 0" << std::endl;
				return mapp::MAPP_BAD_DATA;
		}

		//list available synapse models
		if (vm.count("models")){
			std::cout << "   Following synapse models are available: \n";
			std::cout << "       name           list of accepted parameters\n";
			std::cout << "       tsodyks2       delay, weight, U, u, x, tau_rec, tau_fac\n";
				std::cout << "";
				return mapp::MAPP_USAGE;
			}

		if (vm.count("help")){
			std::cout << desc;
			return mapp::MAPP_USAGE;
		}
		return mapp::MAPP_OK;
	}

	/** \fn content(po::variables_map const& vm)
		\brief Execute the NEST synapse Miniapp.
		\param vm encapsulate the command line and all needed informations
	 */
	void synapse_content(po::variables_map const& vm)
	{
		double dt = vm["dt"].as<double>();
		int iterations = vm["iterations"].as<int>();

		//will turn into ptr to base class if more synapse are implemented
		boost::scoped_ptr<tsodyks2> syn;

		//preallocate vector for results
		spikedetector sd;

		if (vm["model"].as<std::string>() == "tsodyks2") {
			const double delay = vm["delay"].as<double>();
			const double weight = vm["weight"].as<double>();
			const double U = vm["U"].as<double>();
			const double u = vm["u"].as<double>();
			const double x = vm["x"].as<double>();
			const double tau_rec = vm["tau_rec"].as<double>();
			const double tau_fac = vm["tau_fac"].as<double>();

			try {
				syn.reset(new tsodyks2(delay, weight, U, u, x, tau_rec, tau_fac, &sd));
			}
			catch (std::invalid_argument& e) {
				std::cout << "Error in model parameters: " << e.what() << std::endl;
			}
		}
		/* else if () .. further synapse models*/
		else {
			std::cout << "Error: Synapse model implementation missing" << std::endl;
		}

		//create a few events
		std::vector< boost::shared_ptr<spikeevent> > events(iterations);
		for (unsigned int i=0; i<iterations; i++) {
			Time t(i*10.0);

		    events[i].reset(new spikeevent);
		    events[i]->set_stamp( t ); // in Network::send< SpikeEvent >
		    events[i]->set_sender( NULL ); // in Network::send< SpikeEvent >
		    //events[i]->set_sender_gid( sgid ); // Network::send_local
		}

		double t_lastspike = 0.0;
		boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
		for (unsigned int i=0; i<iterations; i++) {
			//send spike
			syn->send(*(event*)events[i].get(), t_lastspike);

			t_lastspike += dt;
		}
		boost::chrono::system_clock::duration delay = boost::chrono::system_clock::now() - start;

		std::cout << "Duration: " << delay << std::endl;
		std::cout << "Last weight " << sd.spikes.back() << std::endl;
	}

	int synapse_execute(int argc, char* const argv[])
	{
		try {
			po::variables_map vm; // it contains everything
			if(int error = synapse_help(argc, argv, vm)) return error;
			synapse_content(vm); // execute the miniapp
		}
		catch(std::exception& e){
			std::cout << e.what() << "\n";
			return mapp::MAPP_UNKNOWN_ERROR;
		}
		return mapp::MAPP_OK; // 0 ok, 1 not ok
	}

};
