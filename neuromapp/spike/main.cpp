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
#include <sstream>
#include <boost/program_options.hpp>
#include <stdlib.h>

#include "spike/spike.h"
#include "utils/error.h"
#include "neuromapp/utils/mpi/mpi_helper.h"

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
    ("numprocs", po::value<size_t>()->default_value(8), "the number of MPI processes")
    ("numthreads", po::value<size_t>()->default_value(8), "the number of OMP threads")
    ("run", po::value<std::string>()->default_value(launcher_helper::mpi_launcher()), "the command to run parallel jobs")
    //("cellgroups", po::value<size_t>()->default_value(64), "the number of cell groups")
    //("eventsper", po::value<size_t>()->default_value(10), "average number of events generated per dt")
    //("percent-ite", po::value<size_t>()->default_value(90), "the percentage of events (per timestep) that are inter-thread")
    ("simtime", po::value<size_t>()->default_value(100), "The number of timesteps in the simulation")
    ("numspike", po::value<size_t>()->default_value(30), "the total number of spikes in the simulation")
    ("numOut", po::value<size_t>()->default_value(4), "number of output presyns (gids) per process")
    ("numIn", po::value<size_t>()->default_value(12), "the number of input presyns per process")
    ("ncper", po::value<size_t>()->default_value(5), "average number of netcons per input presyn")
    ("nonblocking", "If set, use non-blocking algorithm else use default (blocking collective)");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }

    if(vm["numthreads"].as<size_t>() < 1){
	std::cout<<"must execute on at least 1 thread"<<std::endl;
	return mapp::MAPP_BAD_ARG;
    }

    if(vm["numprocs"].as<size_t>() < 1){
	std::cout<<"must execute on at least 1 process"<<std::endl;
	return mapp::MAPP_BAD_ARG;
    }

    if(vm["numOut"].as<size_t>() < 1){
	std::cout<<"must have at least 1 gid per process"<<std::endl;
	return mapp::MAPP_BAD_ARG;
    }

    if(vm["numIn"].as<size_t>() > ((vm["numprocs"].as<size_t>() - 1) * vm["numOut"].as<size_t>())){
	std::cout<<"numIn must be less than or equal to the number of available gids ((numprocs -1) * numOut)"<<std::endl;
	return mapp::MAPP_BAD_ARG;
    }
    /*
    if(vm["percent-ite"].as<size_t>() + vm["percent-spike"].as<size_t>() > 100){
	std::cout<<"percent-ite + percent-spike cannot be greater than 100%"<<std::endl;
	return mapp::MAPP_BAD_ARG;
    }
    */

    return mapp::MAPP_OK;
}

/** \fn spike_content(po::variables_map const& vm)
    \brief Execute the Spike Exchange by calling mpirun/srun on MPI_Exec binary file
    \param vm encapsulate the command line and all needed informations
 */
void spike_content(po::variables_map const& vm){
	std::stringstream command;
    std::string path = helper_build_path::mpi_bin_path();

    command << "OMP_NUM_THREADS=" << vm["numthreads"].as<size_t>() << " " <<
        vm["run"].as<std::string>() <<" -n "<< vm["numprocs"].as<size_t>()<<
        " " << path << "MPI_Exec " //<< vm["cellgroups"].as<size_t>()<< " " <<
        //vm["eventsper"].as<size_t>() << " " << vm["percent-ite"].as<size_t>()
        << " " << vm["simtime"].as<size_t>()<< " " <<
        vm["numspike"].as<size_t>() << " " << vm["numOut"].as<size_t>()
        << " " << vm["numIn"].as<size_t>() << " " << vm["ncper"].as<size_t>()
        << " "<< vm.count("nonblocking");

    std::cout<< "Running command " << command.str() <<std::endl;
	system(command.str().c_str());
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
