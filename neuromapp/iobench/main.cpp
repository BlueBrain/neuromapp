/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * GNU General Public License for more details..  See the GNU
 * Lesser General Public License for more details.
 */

/**
 * @file neuromapp/iobench/main.cpp
 * iobench Miniapp
 */

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <stdlib.h>

#include "iobench/iobench.h"
#include "utils/error.h"
#include "neuromapp/utils/mpi/mpi_helper.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;


/** \fn iobench_help(int argc, char *const argv[], po::variables_map& vm)
    \brief Helper using boost program option to facilitate the command line manipulation
    \param argc number of argument from the command line
    \param argv the command line from the driver or external call
    \param vm encapsulate the command line
    \return error message from mapp::mapp_error
 */
int iobench_help(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "Produce this help message")
    ("backend,b", po::value<std::string>()->default_value("map"), "K/V backend to use")
    ("pairs,n", po::value<unsigned int>()->default_value(1024), "Number of k/v pairs per thread and per iteration")
    ("its,i", po::value<unsigned int>()->default_value(10), "Number of iterations per thread")
    ("skip,s", po::value<unsigned int>()->default_value(0), "Number of initial iterations to discard")
    ("numproc,p", po::value<unsigned int>()->default_value(0), "Number of processes to use. Use 0 to disable MPI")
    ("threads,t", po::value<unsigned int>()->default_value(1), "Number of threads to use")
    ("keySize,k", po::value<size_t>()->default_value(64), "Size of keys in bytes")
    ("valSize,v", po::value<size_t>()->default_value(1024), "Size of values in bytes")
    ("compress,c", po::value<unsigned int>()->default_value(1), "Compress data")
    ("read,r", po::value<unsigned int>()->default_value(1), "Read data")
    ("write,w", po::value<unsigned int>()->default_value(1), "Write data")
    ("rrd", po::value<unsigned int>()->default_value(1), "Read data randomly")
    ("rwr", po::value<unsigned int>()->default_value(1), "Write data randomly") ;

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm["threads"].as<unsigned int>() < 1){
        std::cout << "Error: threads must be at least 1." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["pairs"].as<unsigned int>() < 1){
        std::cout << "Error: pairs must be at least 1." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["its"].as<unsigned int>() < 1){
        std::cout << "Error: its must be at least 1." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["keySize"].as<unsigned int>() < 1){
        std::cout << "Error: keySize must be at least 1." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["valSize"].as<unsigned int>() < 1){
        std::cout << "Error: valSize must be at least 1." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["read"].as<unsigned int>() == 0 && vm["write"].as<int>() == 0){
        std::cout << "Error: please specify at least one operation: read or write." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }

    return mapp::MAPP_OK;
}

/** \fn iobench_content(po::variables_map const& vm)
    \brief Execute the iobench benchmark
    \param vm encapsulate the command line and all needed informations
 */
void iobench_content(po::variables_map const& vm){
    std::stringstream command;
    std::string path = helper_build_path::mpi_bin_path();

    std::string b(vm["backend"].as<std::string>());
    unsigned int n = vm["pairs"].as<unsigned int>();
    unsigned int i = vm["its"].as<unsigned int>();
    unsigned int s = vm["skip"].as<unsigned int>();
    unsigned int p = vm["numproc"].as<unsigned int>();
    unsigned int t = vm["threads"].as<unsigned int>();
    unsigned int k = vm["keySize"].as<unsigned int>();
    unsigned int v = vm["valSize"].as<unsigned int>();
    unsigned int c = vm["compress"].as<unsigned int>();
    unsigned int r = vm["read"].as<unsigned int>();
    unsigned int w = vm["write"].as<unsigned int>();
    unsigned int rrd = vm["rrd"].as<unsigned int>();
    unsigned int rwr = vm["rwr"].as<unsigned int>();

    command << "OMP_NUM_THREADS=" << t << " ";

    if (p > 0) {
        // Launch MPI+OMP app
        command << launcher_helper::mpi_launcher() << " -n " << p << " " << path
                << "MPI_Exec_io";
    } else {
        // Launch simple OMP app
        command << path << "iobench";
    }

    command << " -b " << b << " -n " << n << " -i " << i << " -s " << s << " -k " << k
            << " -v " << v << " -c " << c << " -r " << r << " -w " << w << " --rrd " << rrd
            << " -rwr " << rwr;

	std::cout << "Running command: " << command.str() << std::endl;
	system(command.str().c_str());
}


int iobench_execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = iobench_help(argc, argv, vm)) return error;
        iobench_content(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}
