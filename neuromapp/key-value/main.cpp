/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * @file neuromapp/key-value/main.cpp
 * Key-value Miniapp
 */

#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <stdlib.h>
#include "key-value/key-value.h"
#include "utils/error.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;


int toto(int argc , char* argv[]);

/** \fn keyvalue_help(int argc, char *const argv[], po::variables_map& vm)
    \brief Helper using boost program option to facilitate the command line manipulation
    \param argc number of argument from the command line
    \param argv the command line from the driver or external call
    \param vm encapsulate the command line
    \return error message from mapp::mapp_error
 */
int keyvalue_help(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "Produce this help message")

    ("numproc,p", po::value<int>()->default_value(1), "[int]    Number of MPI processes")
    ("numthread,t", po::value<int>()->default_value(1), "[int]    Number of OpenMP threads")

    ("backend,b", po::value<std::string>()->default_value("map"), "[string] Key-value store backend to use. \
                  Supported backends: map (STL map), skv (IBM SKV), ldb (LevelDB)")
    ("async,a", "If available, use the asynchronous API of the key-value backend")
    ("flash,f", "If available, use flash memory as storage")
    ("usecase,u", po::value<int>()->default_value(1), "[int] Use case to simulate. Creates a data set size of: \
                  1 (25% BG/Q node DRAM, 4 GB), 2 (50% DRAM, 8 GB), 3 (75% DRAM, 12 GB)")
    ("st", po::value<float>()->default_value(1.0), "[float]  Simulation time in ms")
    ("md", po::value<float>()->default_value(0.1), "[float]  Min delay in ms")
    ("dt", po::value<float>()->default_value(0.025), "[float]  Delta time in ms")
    ("cg", po::value<int>()->default_value(0), "[int]    Number of cell groups per process, \
                  must be at least equal to the number of OpenMP threads, --numthread by default");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm["numproc"].as<int>() < 1){
        std::cout << "Error: numproc must be at least 1." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["numthread"].as<int>() < 1){
        std::cout << "Error: numthread must be at least 1." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if((vm["backend"].as<std::string>() != "map")
            && (vm["backend"].as<std::string>() != "skv")
            && (vm["backend"].as<std::string>() != "ldb")){
        std::cout << "Error: backend " << vm["backend"].as<std::string>()
                << " not supported." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["usecase"].as<int>() < 1 || vm["usecase"].as<int>() > 3){
        std::cout << "Error: usecase must be between 1 and 3." << std::endl;
	    return mapp::MAPP_BAD_ARG;
    }

    if(vm["md"].as<float>() > vm["st"].as<float>()){
        std::cout << "Error: md cannot be greater than st." << std::endl;
	    return mapp::MAPP_BAD_ARG;
    }

    if(vm["dt"].as<float>() > vm["md"].as<float>()){
        std::cout << "Error: dt cannot be greater than md." << std::endl;
	    return mapp::MAPP_BAD_ARG;
    }

    if(vm["cg"].as<int>() != 0 && vm["cg"].as<int>() < vm["numthread"].as<int>()){
        std::cout << "Error: number of cell groups (" << vm["cg"].as<int>()
                << ") must be at least equal to the number of OpenMP threads ("
                << vm["numthread"].as<int>() << ")." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }


    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }

    return mapp::MAPP_OK;
}

/** \fn keyvalue_content(po::variables_map const& vm)
    \brief Execute the Key-value benchmark
    \param vm encapsulate the command line and all needed informations
 */
void keyvalue_content(po::variables_map const& vm){

    int np = vm["numproc"].as<int>();
    int nt = vm["numthread"].as<int>(); // Should we check OMP_NUM_THREADS env var also? --> 'char * getenv (const char *name)'
    std::string backend(vm["backend"].as<std::string>());
    bool async = !(vm["async"].empty());
    bool flash = !(vm["flash"].empty());
    int uc = vm["usecase"].as<int>();
    float st = vm["st"].as<float>();
    float md = vm["md"].as<float>();
    float dt = vm["dt"].as<float>();
    int cg = vm["cg"].as<int>();

    // Last checks
    if (cg == 0) {
    	cg = nt;
    }

    std::stringstream command;

    command << "OMP_NUM_THREADS=" << nt << " mpirun -n " << np
            << " /Users/ewart/Documents/miniapps/b/neuromapp/key-value/MPI_Exec_kv  -b " << backend
    		<< " " << (async ? "-a" : "") << " " << (flash ? "-f" : "") << " -uc " << uc << " -st " << st
    		<< " -md " << md << " -dt " << dt << " -cg " << cg;


    //command << "srun -n 4 neuromapp/key-value/mpiexec";

	//char command[130];
	//snprintf(command, 130, "OMP_NUM_THREADS=%d srun -n %d neuromapp/key-value/MPI_Exec_kv -b %s %s %s -uc %d -st %f -md %f -dt %f -cg %d",
	//	nt, np, backend.c_str(), (async ? "-a" : ""), (flash ? "-f" : ""), uc, st, md, dt, cg);

	std::cout << "Running command " << command.str() << std::endl;

	system(command.str().c_str());

}


int keyvalue_execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = keyvalue_help(argc, argv, vm)) return error;
        keyvalue_content(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}
