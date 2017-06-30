/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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
 * @file neuromapp/hello/main.cpp
 * \brief Hello Word Miniapp
 */

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "readi/readi.h"
#include "utils/error.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;

/** \fn help(int argc, char *const argv[], po::variables_map& vm)
    \brief Helper using boost program option to facilitate the command line manipulation
    \param argc number of argument from the command line
    \param argv the command line from the driver or external call
    \param vm encapsulate the command line
    \return error message from mapp::mapp_error
 */
int readi_help(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message");
//    ("name", po::value<std::string>()->default_value("world"), "who ?")
//    ("numthread", po::value<int>()->default_value(1), "number of OMP thread");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

//    omp_set_num_threads(vm["numthread"].as<int>());

    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }
    return mapp::MAPP_OK;
}

/** \fn content(po::variables_map const& vm)
    \brief Execute the hello World
    \param vm encapsulate the command line and all needed informations
 */
void readi_content(po::variables_map const& vm){
/*    #pragma omp parallel
    #pragma omp critical
    {
        std::cout << "Hello " << vm["name"].as<std::string>()
                  << ", total thread: " << vm["numthread"].as<int>()
                  << ", thread id: " << omp_get_thread_num() << "\n";
    }
*/
    std::cout << "ca va ou bien ?" << std::endl;
}

int readi_execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = readi_help(argc, argv, vm)) return error;
        readi_content(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}
