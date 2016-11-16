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
 * @file neuromapp/replib/main.cpp
 * replib Miniapp
 */

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <stdlib.h>

#include "replib/replib.h"
#include "utils/error.h"
#include "neuromapp/utils/mpi/mpi_helper.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;


/** \fn replib_help(int argc, char *const argv[], po::variables_map& vm)
    \brief Helper using boost program option to facilitate the command line manipulation
    \param argc number of argument from the command line
    \param argv the command line from the driver or external call
    \param vm encapsulate the command line
    \return error message from mapp::mapp_error
 */
int replib_help(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "Produce this help message")

    ("numproc,p", po::value<int>()->default_value(1), "[int]    Number of MPI processes")

    ("write,w", po::value<std::string>()->default_value("rnd1b"), "[string] Specify how processes write to file. Supported options:\n- rnd1b (random, 1 block per process),\n- file1b (read distribution from file, but write 1 block per process),\n- fileNb (read distribution from file, reproduce same block assignment)")

    ("output,o", po::value<std::string>()->default_value("./currents.bbp"), "[string] Path and name of the output report file.")

    ("file,f", po::value<std::string>()->default_value(""), "[string] Path to the file with write distribution per process")

    ("invert,i", "Invert the rank ID, only applies when reading distribution from file (fileXX)")

    ("numcells,c", po::value<int>()->default_value(10), "[int] Number of cells, only applies when creating random write distribution (rndXX)")

    ("sim-steps,s", po::value<int>()->default_value(15), "[int] Number of simulation steps for each reporting step")

    ("rep-steps,r", po::value<int>()->default_value(1), "[int] Number of reporting steps (1 reporting step happens every 'sim-steps' simulation steps)")

    ("time,t", po::value<int>()->default_value(100), "[int] Amount of time spent in 1 simulation step (in ms)")

    ("check,v", "Verify the output report was correctly written");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm["numproc"].as<int>() < 1){
        std::cout << "Error: numproc must be at least 1." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if((vm["write"].as<std::string>() != "rnd1b")
            && (vm["write"].as<std::string>() != "file1b")
            && (vm["write"].as<std::string>() != "fileNb")){
        std::cout << "Error: write distribution " << vm["write"].as<std::string>()
                << " not supported." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["output"].as<std::string>() == ""){
        std::cout << "Error: Output report path cannot be empty." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["write"].as<std::string>() != "rnd1b" && vm["file"].as<std::string>() == ""){
        std::cout << "Error: The given writing option requires a write distribution file." << std::endl;
        return mapp::MAPP_BAD_ARG;
    }

    if(vm["numcells"].as<int>() < 1 ){
        std::cout << "Error: numcells must be at least 1." << std::endl;
           return mapp::MAPP_BAD_ARG;
    }

    if(vm["sim-steps"].as<int>() < 1 ){
        std::cout << "Error: simulation steps must be at least 1." << std::endl;
           return mapp::MAPP_BAD_ARG;
    }

    if(vm["rep-steps"].as<int>() < 1 ){
        std::cout << "Error: reporting steps must be at least 1." << std::endl;
           return mapp::MAPP_BAD_ARG;
    }

    if(vm["time"].as<int>() < 0 ){
        std::cout << "Error: time cannot be a negative number." << std::endl;
           return mapp::MAPP_BAD_ARG;
    }

    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }

    return mapp::MAPP_OK;
}

/** \fn replib_content(po::variables_map const& vm)
    \brief Execute the replib benchmark
    \param vm encapsulate the command line and all needed informations
 */
void replib_content(po::variables_map const& vm){
    std::stringstream command;
    std::string path = helper_build_path::mpi_bin_path();

    int np = vm["numproc"].as<int>();
    std::string wr(vm["write"].as<std::string>());
    std::string o(vm["output"].as<std::string>());
    std::string f(vm["file"].as<std::string>());
    bool inv = !(vm["invert"].empty());
    int nc = vm["numcells"].as<int>();
    int ssteps = vm["sim-steps"].as<int>();
    int rsteps = vm["rep-steps"].as<int>();
    int t = vm["time"].as<int>();
    bool check = !(vm["check"].empty());

    command << launcher_helper::mpi_launcher() << " -n " << np << " " << path
            << "MPI_Exec_rl  -w " << wr << " -o " << o << ((f == "") ? "" : " -f ") << f
            << " " << (inv ? "-i" : "") << " " << " -c " << nc << " -s " << ssteps
            << " -r " << rsteps << " -t " << t << (check ? " -v" : "");

	std::cout << "Running command: " << command.str() << std::endl;
	system(command.str().c_str());
}


int replib_execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = replib_help(argc, argv, vm)) return error;
        replib_content(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}
