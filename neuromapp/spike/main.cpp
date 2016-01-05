/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
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
 * @file neuromapp/spike/main.cpp
 * Spike Exchange Miniapp
 */

#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <stdlib.h>
#include "spike/spike.h"
#include "utils/error.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;

/** \fn spike_help(int argc, char *const argv[], po::variables_map& vm)
    \brief Helper using boost program option to facilitate the command line manipulation
    \param argc number of argument from the command line
    \param argv the command line from the driver or external call
    \param vm encapsulate the command line
    \return error message from mapp::mapp_error
 */
int spike_help(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce this help message")
    ("numproc", po::value<int>()->default_value(1), "the number of MPI processes")
    ("eventsper", po::value<int>()->default_value(3), "average number of events to be generated per dt")
    ("gidsper", po::value<int>()->default_value(3), "number of gids per process")
    ("simtime", po::value<int>()->default_value(10), "The number of timesteps in the simulation")
    ("dt", po::value<int>()->default_value(1), "the average time (in UNITS) per dt")
    ("sdev", po::value<int>()->default_value(0), "the standard deviation for dt");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm["numproc"].as<int>() < 1){
	return mapp::MAPP_BAD_ARG;
    }

    if(vm["eventsper"].as<int>() < 0){
	return mapp::MAPP_BAD_ARG;
    }

    if(vm["gidsper"].as<int>() < 1){
	return mapp::MAPP_BAD_ARG;
    }

    if(vm["simtime"].as<int>() < 0){
	return mapp::MAPP_BAD_ARG;
    }

    if(vm["dt"].as<int>() < 1){
	return mapp::MAPP_BAD_ARG;
    }

    if(vm["sdev"].as<int>() < 0){
	return mapp::MAPP_BAD_ARG;
    }

    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }
    return mapp::MAPP_OK;
}

/** \fn spike_content(po::variables_map const& vm)
    \brief Execute the Spike Exchange
    \param vm encapsulate the command line and all needed informations
 */
void spike_content(po::variables_map const& vm){
        std::cout << "Run MPI Hello World:" <<std::endl;
	char command[50];
	snprintf(command, 50, "mpirun -n %d neuromapp/spike/MPI_Exec %d %d %d %d %d",
		vm["numproc"].as<int>(), vm["eventsper"].as<int>(), vm["gidsper"].as<int>(),
		vm["simtime"].as<int>(), vm["dt"].as<int>(), vm["sdev"].as<int>());
	system(command);
}

int spike_execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = spike_help(argc, argv, vm)) return error;
        spike_content(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}
