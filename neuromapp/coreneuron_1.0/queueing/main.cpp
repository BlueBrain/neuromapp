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
    ("numthread", po::value<int>()->default_value(1),
     "number of OMP thread")
    ("eventsper", po::value<int>()->default_value(50),
     "number of events created per time step")
    ("simtime", po::value<int>()->default_value(5000),
     "number of time steps in the simulation")
    ("percent-ite", po::value<int>()->default_value(90),
     "the percentage of inter-thread events out of total events")
    ("spike-enabled","determines whether or not to include spike events")
    ("verbose","provides additional outputs during execution")
    ("spinlock","runs the simulation using spinlocks/linked-list instead of mutexes/vector")
    ("with-algebra","simulation performs linear algebra calculations");
    //future options : fraction of interthread events

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }

    if(vm["numthread"].as<int>() < 1)
		return mapp::MAPP_BAD_ARG;

    if(vm["eventsper"].as<int>() < 1)
		return mapp::MAPP_BAD_ARG;

    if(vm["simtime"].as<int>() < 1)
		return mapp::MAPP_BAD_ARG;

   if( (vm["percent-ite"].as<int>() < 0) || (vm["percent-ite"].as<int>() > 100) )
		return mapp::MAPP_BAD_ARG;

#ifdef _OPENMP
    omp_set_num_threads(vm["numthread"].as<int>());
#endif
    return mapp::MAPP_OK;
}

/**
 * \fnrun_sim
 * \brief the actual queueing simulation
 */
template<implementation I>
void run_sim(pool<I> &pl, po::variables_map const&vm){
    struct timeval start, end;
	pl.generate_all_events(vm["simtime"].as<int>());
    gettimeofday(&start, NULL);
    for(int j = 0; j < vm["simtime"].as<int>(); ++j){
        pl.time_step();
		pl.handle_spike(vm["simtime"].as<int>());
    }
	pl.accumulate_stats();
    gettimeofday(&end, NULL);
    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec)) + ((end.tv_usec - start.tv_usec) / 1000);
	std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;
}

/** \fn queueing_miniapp(po::variables_map const& vm)
    \brief Execute the queing miniapp
    \param vm encapsulate the command line and all needed informations
 */
void queueing_miniapp(po::variables_map const& vm){
	bool verbose = vm.count("verbose");
	bool spike = vm.count("spike-enabled");
	bool algebra = vm.count("with-algebra");

    if(vm.count("spinlock")){
		pool<spinlock> pl(verbose, vm["eventsper"].as<int>(), vm["percent-ite"].as<int>(), spike, algebra);
		run_sim(pl,vm);
    } else {
		pool<mutex> pl(verbose, vm["eventsper"].as<int>(), vm["percent-ite"].as<int>(), spike, algebra);
		run_sim(pl,vm);
    }
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

