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
int event_help(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce this help message")
    ("numprocs", po::value<size_t>()->default_value(8),
    "the number of MPI processes")
    ("numthreads", po::value<size_t>()->default_value(8),
    "the number of OMP threads")
    ("run", po::value<std::string>()->default_value(launcher_helper::mpi_launcher()),
    "the command to run parallel jobs")
    ("numgroups", po::value<size_t>()->default_value(64),
    "the number of cell groups")
    ("simtime", po::value<size_t>()->default_value(100),
    "The number of timesteps in the simulation")
    ("numspikes", po::value<size_t>()->default_value(30),
    "the number of spikes generated per process")
    ("numites", po::value<size_t>()->default_value(30),
    "the number of inter-thread generated per process")
    ("numlocals", po::value<size_t>()->default_value(30),
    "the number of spikes generated per process")
    ("numout", po::value<size_t>()->default_value(4),
    "number of output presyns (gids) per process")
    ("numin", po::value<size_t>()->default_value(12),
    "the number of input presyns per process")
    ("ncper", po::value<size_t>()->default_value(5),
    "average number of netcons per input presyn")
    ("algebra",
    "If set, perform linear algebra");

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

    if(vm["numout"].as<size_t>() < 1){
	std::cout<<"must have at least 1 gid per process"<<std::endl;
	return mapp::MAPP_BAD_ARG;
    }

    if(vm["numin"].as<size_t>() >
       ((vm["numprocs"].as<size_t>() - 1) * vm["numout"].as<size_t>())){
	std::cout<<"numin must be less than or equal to the number of\
available gids ((numprocs -1) * numout)"<<std::endl;
	return mapp::MAPP_BAD_ARG;
    }
    return mapp::MAPP_OK;
}

/** \fn event_content(po::variables_map const& vm)
    \brief Execute event passing app by calling mpirun/srun on binary file
    \param vm encapsulate the command line and all needed informations
 */
void event_content(po::variables_map const& vm){
    std::stringstream command;
    std::string path = helper_build_path::mpi_bin_path();


    size_t nthread = vm["numthreads"].as<size_t>();
    std::string mpi_run = vm["run"].as<std::string>();
    size_t nproc = vm["numprocs"].as<size_t>();

    //command line args
    size_t ngroup = vm["numgroups"].as<size_t>();
    size_t simtime = vm["simtime"].as<size_t>();
    size_t nout = vm["numout"].as<size_t>();
    size_t nin = vm["numin"].as<size_t>();
    size_t ncper = vm["ncper"].as<size_t>();
    size_t nspike = vm["numspikes"].as<size_t>();
    size_t nite = vm["numites"].as<size_t>();
    size_t nlocal = vm["numlocals"].as<size_t>();
    size_t algebra = vm.count("algebra");

    command << "OMP_NUM_THREADS=" << nthread << " " <<
        mpi_run <<" -n "<< nproc << " " << path << "event_exec " <<
        ngroup << " " << simtime << " " <<
        nout << " " << nin << " " << ncper << " " <<
        nspike << " " << nite << " " << nlocal << " " << algebra;

    std::cout<< "Running command " << command.str() <<std::endl;
	system(command.str().c_str());
}

int event_execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = event_help(argc, argv, vm)) return error;
        event_content(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}
