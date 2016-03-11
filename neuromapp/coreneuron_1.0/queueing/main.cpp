/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/queueing/main.cpp
 * \brief Handles queueing for inter-thread event exchange
 */

#include <iostream>
#include <ctime>
#include <boost/program_options.hpp>
#include <sys/time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "coreneuron_1.0/queueing/pool.h"
#include "coreneuron_1.0/queueing/thread.h"
#include "coreneuron_1.0/queueing/queueing.h"
#include "utils/error.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;

namespace queueing {

/** \fn qhelp(int argc, char *const argv[], po::variables_map& vm)
 *  \brief Helper using boost program option to facilitate the command line manipulation
 *  \param argc number of argument from the command line
 *  \param argv the command line from the driver or external call
 *  \param vm encapsulate the command line
 *  \return error message from mapp::mapp_error
 */
int qhelp(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "produce help message")
    ("nthreads,n", po::value<int>()->default_value(1),
     "number of OMP thread")
    ("cell-groups,c", po::value<int>()->default_value(64),
	 "number of cell groups in the simulation")
    ("num-local,l", po::value<int>()->default_value(50000),
     "number of local events")
    ("time,t", po::value<int>()->default_value(5000),
     "number of time steps in the simulation")
    ("num-ite,i", po::value<int>()->default_value(0),
     "the number of inter-thread events")
    ("verbose,v","provides additional outputs during execution")
    ("with-algebra,a","simulation performs linear algebra calculations");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }
    if(vm["nthreads"].as<int>() < 1){
        std::cout<<"numthread must be non-zero"<<std::endl;
        return mapp::MAPP_BAD_ARG;
    }
    if(vm["cell-groups"].as<int>() < vm["nthreads"].as<int>()){
        std::cout<<"cell-groups should be >= numthreads"<<std::endl;
        return mapp::MAPP_BAD_ARG;
    }
    if(vm["num-local"].as<int>() < 0){
        std::cout<<"num-local must be non-zero"<<std::endl;
        return mapp::MAPP_BAD_ARG;
    }
    if(vm["time"].as<int>() < 1){
        std::cout<<"time must be non-zero"<<std::endl;
        return mapp::MAPP_BAD_ARG;
    }
   if( (vm["num-ite"].as<int>() < 0)){
        std::cout<<"num-ite cannot be negative"<<std::endl;
        return mapp::MAPP_BAD_ARG;
    }
   if( (vm["num-ite"].as<int>() + vm["num-local"].as<int>() == 0)){
        std::cout<<"running test with no events"<<std::endl;
        return mapp::MAPP_BAD_ARG;
    }

#ifdef _OPENMP
    omp_set_num_threads(vm["nthreads"].as<int>());
#endif
    return mapp::MAPP_OK;
}

/** \fn queueing_miniapp(po::variables_map const& vm)
 *  \brief Execute the queing miniapp
 *  \param vm encapsulate the command line and all needed informations
 */
void queueing_miniapp(po::variables_map const& vm){
    int cellgroups = vm["cell-groups"].as<int>();
    int n_ite = vm["num-ite"].as<int>();
    int n_local = vm["num-local"].as<int>();
    bool verbose = vm.count("verbose");
    bool algebra = vm.count("with-algebra");
    int simtime = vm["time"].as<int>();
    pool pl(cellgroups, n_local, n_ite, simtime, verbose, algebra);

    struct timeval start, end;
    pl.generate_all_events();
    gettimeofday(&start, NULL);

    for(int j = 0; j < (simtime/pl.mindelay()); ++j){
        pl.fixed_step();
    }
    pl.accumulate_stats();

    gettimeofday(&end, NULL);
    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec)) + ((end.tv_usec - start.tv_usec) / 1000);
    std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;
}

} //end namespace
int queueing_execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = queueing::qhelp(argc, argv, vm)) return error;
        queueing::queueing_miniapp(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}

