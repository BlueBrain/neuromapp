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
 * @file neuromapp/event_passing/drivers/main.cpp
 * Event Passing Miniapp
 */

#include <iostream>
#include <string>
#include <sstream>
#include <boost/program_options.hpp>
#include <stdlib.h>

#include "nest/drivers/event_passing.h"

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
int nest_event_passing_help(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce this help message")
    ("numprocs", po::value<size_t>()->default_value(8),"the number of MPI processes")
    ("numthreads", po::value<size_t>()->default_value(8),"the number of OMP threads per process")
    ("run", po::value<std::string>()->default_value(launcher_helper::mpi_launcher()),"the command to run parallel jobs")
    ("simtime", po::value<size_t>()->default_value(100),"the number of timesteps in the simulation")
    ("numcells", po::value<size_t>()->default_value(64),"total number of presynaptic cells (gids) in the simulation")
    ("fan", po::value<size_t>()->default_value(12),"the number of synapses per neuron")
    ("numspikes", po::value<size_t>()->default_value(30),"Total number of spikes produced by the simulation")
    ("mindelay", po::value<size_t>()->default_value(3),"the number of timesteps per fixed step function")

    ("model", po::value<std::string>()->default_value("tsodyks2"), "select synapse model")
    // tsodyks2 parameters
    ("delay", po::value<double>()->default_value(1.0), "tsodyks2 model parameter: delay")
    ("weight", po::value<double>()->default_value(1.0), "tsodyks2 model parameter: weight")
    ("U", po::value<double>()->default_value(0.5), "tsodyks2 model parameter: U")
    ("u", po::value<double>()->default_value(0.5), "tsodyks2 model parameter: u")
    ("x", po::value<double>()->default_value(1), "tsodyks2 model parameter: x")
    ("tau_rec", po::value<double>()->default_value(800.0), "tsodyks2 model parameter: tau_rec")
    ("tau_fac", po::value<double>()->default_value(0.0), "tsodyks2 model parameter: tau_fac")

    ("pool", po::value<bool>()->default_value(false), "use pool implementation");

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

    if(vm["numcells"].as<size_t>() < vm["numprocs"].as<size_t>()){
	std::cout<<"must have at least 1 gid per process"<<std::endl;
	return mapp::MAPP_BAD_ARG;
    }

    return mapp::MAPP_OK;
}

/** \fn event_content(po::variables_map const& vm)
    \brief Execute event passing app by calling mpirun/srun on binary file
    \param vm encapsulate the command line and all needed informations
 */
void nest_event_passing_content(po::variables_map const& vm){
    std::stringstream command;
    std::string path = helper_build_path::mpi_bin_path();

    size_t nthread = vm["numthreads"].as<size_t>();
    std::string mpi_run = vm["run"].as<std::string>();
    size_t nproc = vm["numprocs"].as<size_t>();

    //command line args
    size_t simtime = vm["simtime"].as<size_t>();
    size_t ncells = vm["numcells"].as<size_t>();
    size_t fan = vm["fan"].as<size_t>();
    size_t nspike = vm["numspikes"].as<size_t>();
    size_t mindelay = vm["mindelay"].as<size_t>();

    std::string syn_model = vm["model"].as<std::string>();
    double syn_delay = vm["delay"].as<double>();
    double syn_weight = vm["weight"].as<double>();
    double syn_U = vm["U"].as<double>();
    double syn_u = vm["u"].as<double>();
    double syn_x = vm["x"].as<double>();
    double syn_tau_rec = vm["tau_rec"].as<double>();
    double syn_tau_fac = vm["tau_fac"].as<double>();
    bool pool = vm["pool"].as<bool>();

    std::string exec ="nest_event_passing_distributed_exec";

    command << "OMP_NUM_THREADS=" << nthread << " " <<
        mpi_run <<" -n "<< nproc << " " << path << exec << " " <<
        nthread << " " << simtime << " " <<
        ncells << " " << fan << " " <<
        nspike << " " << mindelay << " " <<
        syn_model << " " << syn_delay << " " <<
        syn_weight << " " << syn_U << " " <<
        syn_u << " " << syn_x << " " <<
        syn_tau_rec << " " << syn_tau_fac << " " << pool;

    std::cout<< "Running command " << command.str() <<std::endl;
	system(command.str().c_str());
}

int nest::event_passing::execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = nest_event_passing_help(argc, argv, vm)) return error;
        nest_event_passing_content(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}
