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
 * @file neuromapp/queueing/main.cpp
 * Handles queueing for inter-thread event exchange
 */

#include <iostream>
#include <string>
#include <time.h>
#include <ctime>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <boost/program_options.hpp>

#include "queueing/pool.h"
#include "queueing/queueing.h"
#include "utils/error.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;

/** \fn qhelp(int argc, char *const argv[], po::variables_map& vm)
    \brief Helper using boost program option to facilitate the command line manipulation
    \param argc number of argument from the command line
    \param argv the command line from the driver or external call
    \param vm encapsulate the command line
    \return error message from mapp::mapp_error
 */
int qhelp(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("numthread", po::value<int>()->default_value(2),
     "number of OMP thread (must be greater than 1)")
    ("eventsper", po::value<int>()->default_value(20),
     "number of events created per time step.")
    ("simtime", po::value<int>()->default_value(5000),
     "number of time steps in the simulation")
    ("verbose","provides additional outputs during execution");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }

#ifdef _OPENMP
    if(vm["numthread"].as<int>() < 2){
	return mapp::MAPP_BAD_ARG;
    }
    omp_set_num_threads(vm["numthread"].as<int>());
#endif
    return mapp::MAPP_OK;
}

/** \fn queueing_miniapp(po::variables_map const& vm)
    \brief Execute the queing miniapp
    \param vm encapsulate the command line and all needed informations
 */
void queueing_miniapp(po::variables_map const& vm){
    std::clock_t start;
    double duration;
    Pool pl(vm["numthread"].as<int>(), vm.count("verbose"),
		    vm["eventsper"].as<int>());

    start = std::clock();
    for(int i = 0; i < vm["simtime"].as<int>(); ++i){
	pl.checkThresh(vm["simtime"].as<int>());
	pl.enqueueAll();
	pl.deliverAll();
    }
    duration = (std::clock() - start ) / (double) CLOCKS_PER_SEC;

    std::cout<<"Time: "<<duration<<std::endl;
}

int queueing_execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = qhelp(argc, argv, vm)) return error;
        queueing_miniapp(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}

