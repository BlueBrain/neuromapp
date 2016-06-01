/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
 * Till Schumann - Swiss Federal Institute of technology in Lausanne,
 * till.schumann@epfl.ch,
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
 * @file neuromapp/nest/synapse/drivers/connector_main.cpp
 * \brief NEST controller Miniapp
 */

#include <iostream>
#include <string>


#include <boost/program_options.hpp>
#include <boost/chrono.hpp>
#include <boost/scoped_ptr.hpp>

#include "nest/synapse/drivers/synapse.h"
#include "nest/synapse/event.h"
#include "nest/synapse/connector_base.h"
#include "nest/synapse/scheduler.h"
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
    int connector_help(int argc, char* const argv[], po::variables_map& vm)
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

        ("num_synapses", po::value<int>()->default_value(1), "number of synapses per connector")

        // simulation parameters
        ("dt", po::value<double>()->default_value(0.1), "time between spikes")
        ("iterations", po::value<int>()->default_value(1), "number of iterations (spikes)");


        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        //check for valid synapse model & parameters
        /* else if ( more models ) */
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
                short lid = -1;
                tsodyks2 syn(delay, weight, U, u, x, tau_rec, tau_fac, lid);
            }
            catch (std::invalid_argument& e) {
                std::cout << "Error in model parameters: " << e.what() << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
        }

        //check for valid synapse number
        if (vm["num_synapses"].as<int>() < 1) {
                std::cout << "Error: Number of synapses per connector has to be a greater than 0" << std::endl;
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
    void connector_content(po::variables_map const& vm)
    {
        const double dt = vm["dt"].as<double>();
        const int iterations = vm["iterations"].as<int>();
        const int num_detectors = vm["num_synapses"].as<int>();

        //preallocate vector for results
        std::vector<spikedetector> detectors(num_detectors);
        std::vector<targetindex> detectors_targetindex(num_detectors);
        scheduler sch;

        // register spike detectors
        for(int i =  0; i < num_detectors; ++i) {
            detectors[i].set_lid(i);    //give nodes a local id
            detectors_targetindex[i] = scheduler::add_node(&detectors[i]);  //add them to the scheduler
        }

        //create a few spike events
        std::vector< boost::shared_ptr<spikeevent> > events(iterations);
        for (unsigned int i=0; i<iterations; i++) {
            Time t(i*10.0);
            events[i].reset(new spikeevent);
            events[i]->set_stamp( t ); // in Network::send< SpikeEvent >
        }

        //grow the connector
        //for num_detectors > K_CUTOFF connector turns from static to dynamic

        const double delay = vm["delay"].as<double>();
        const double weight = vm["weight"].as<double>();
        const double U = vm["U"].as<double>();
        const double u = vm["u"].as<double>();
        const double x = vm["x"].as<double>();
        const double tau_rec = vm["tau_rec"].as<double>();
        const double tau_fac = vm["tau_fac"].as<double>();
        tsodyks2 synapse(delay, weight, U, u, x, tau_rec, tau_fac, detectors_targetindex[0]);
        ConnectorBase* conn = new Connector<1,tsodyks2>(synapse);
        for(unsigned int i = 1; i < num_detectors; ++i) {
            tsodyks2 synapse(delay, weight, U, u, x, tau_rec, tau_fac, detectors_targetindex[i]);
            conn = &((vector_like<tsodyks2>*)conn)->push_back(synapse);
        }

        //ref NEST ConnectionManager::send( thread t, index sgid, Event& e )
        // simulated line: connections_[ t ].get( sgid )->send( e, t, prototypes_[ t ] );
        double t_lastspike = 0.0;
        boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
        for (unsigned int i=0; i<iterations; i++) {
            //send spike
            conn->send(*(event*)events[i].get(), t_lastspike);
        }
        boost::chrono::system_clock::duration duration = boost::chrono::system_clock::now() - start;

        std::cout << "Duration: " << duration << std::endl;
        std::cout << "Detectors size: " << detectors.size() << std::endl;
        for(unsigned int i=0; i<num_detectors; ++i) {
            std::cout<<"Detector "<<i<<": ";
            for(unsigned int j=0; j<detectors[i].spikes.size(); ++j) {
                std::cout << detectors[i].spikes[j].get_weight() << " ";
            }
            std::cout<<std::endl;
        }

        delete conn;
    }

    int connector_execute(int argc, char* const argv[])
    {
        try {
            po::variables_map vm; // it contains everything
            if(int error = connector_help(argc, argv, vm)) return error;
            connector_content(vm); // execute the miniapp
        }
        catch(std::exception& e){
            std::cout << e.what() << "\n";
            return mapp::MAPP_UNKNOWN_ERROR;
        }
        return mapp::MAPP_OK; // 0 ok, 1 not ok
    }

}
