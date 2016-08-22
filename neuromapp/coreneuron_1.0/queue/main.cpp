/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
 * timothee ewart - Swiss Federal Institute of technology in Lausanne,
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
 * GNU General Public License for more details..  See the GNU
 * Lesser General Public License for more details.
 */

/**
 * @file neuromapp/keyvalue/main.cpp
 * keyvalue Miniapp
 */

#include <iostream>
#include <fstream>
#include <string>

#include "utils/error.h"

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

#include "coreneuron_1.0/queue/tool/priority_queue.hpp"
#include "coreneuron_1.0/queue/trait.h"
#include "coreneuron_1.0/queue/serial_benchmark.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;

/** \fn keyvalue_help(int argc, char *const argv[], po::variables_map& vm)
 \brief Helper using boost program option to facilitate the command line manipulation
 \param argc number of argument from the command line
 \param argv the command line from the driver or external call
 \param vm encapsulate the command line
 \return error message from mapp::mapp_error
 */
int queue_help(int argc, char* const argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("benchmark", po::value<std::string>()->default_value("push"), "push, pop, push_one, mh_bench or all")
    ("size", po::value<int>()->default_value(10), "bench = 2^size")
    ("io", po::value<bool>()->default_value(false), "save $benchmark results IO i.e. pop.csv");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")){
        std::cout << desc;
        return mapp::MAPP_USAGE;
    }

    return mapp::MAPP_OK;
}

template<class T>
void benchmark(int iteration, bool io){
    int size(1);
    std::list<std::string> res;
    res.push_back("#elements,std::priority_queue,sptq_queue,bin_queue,boost::binomial_heap,boost::fibonacci_heap,boost::skew_heap,boost::pairing_heap \n");

    for(int i=1; i< iteration; ++i){
        std::string bench = boost::lexical_cast<std::string>(size) + ",";
        bench += boost::lexical_cast<std::string>(T::template benchmark<helper_type<priority_queue> >(size));
        bench += boost::lexical_cast<std::string>(T::template benchmark<helper_type<sptq_queue> >(size)) + ",";
        bench += boost::lexical_cast<std::string>(T::template benchmark<helper_type<bin_queue> >(size)) + ",";
        bench += boost::lexical_cast<std::string>(T::template benchmark<helper_type<binomial_heap> >(size)) + ",";
        bench += boost::lexical_cast<std::string>(T::template benchmark<helper_type<fibonacci_heap> >(size)) + ",";
        bench += boost::lexical_cast<std::string>(T::template benchmark<helper_type<skew_heap> >(size)) + ",";
        bench += boost::lexical_cast<std::string>(T::template benchmark<helper_type<d_ary_heap> >(size)) + "\n";
        res.push_back(bench);
        size<<=1; // 1,2,4,8 ....
    }

    std::fstream out;
    std::string name_file = boost::lexical_cast<std::string>(T::name) + ".csv";
    out.open(name_file.c_str(),std::fstream::out);
    std::copy(res.begin(),res.end(), std::ostream_iterator<std::string>(std::cout, " ")); //screen
    if(!io)
        std::copy(res.begin(),res.end(), std::ostream_iterator<std::string>(out)); //io
}


/** \fn keyvalue_content(po::variables_map const& vm)
 \brief Execute the keyvalue benchmark
 \param vm encapsulate the command line and all needed informations
 */
int queue_content(po::variables_map const& vm){

    std::string bench = vm["benchmark"].as<std::string>();

    int iteration = vm["size"].as<int>();
    if(iteration < 0)
        return mapp::MAPP_BAD_ARG;

    bool io = vm["io"].as<bool>();

    std::map<std::string,queue::benchs> m;
    m.insert(std::make_pair("push",queue::push));
    m.insert(std::make_pair("pop",queue::push));
    m.insert(std::make_pair("push_one",queue::push));
    m.insert(std::make_pair("mh_bench",queue::push));
    m.insert(std::make_pair("all",queue::push));

    switch(m[bench]){
        case queue::push :
            benchmark<queue::push_helper>(iteration,io);
            break;
        case queue::pop :
            benchmark<queue::pop_helper>(iteration,io);
            break;
        case queue::push_one :
            benchmark<queue::push_one_helper>(iteration,io);
            break;
        case queue::mh_bench :
            benchmark<queue::mhines_bench_helper>(iteration,io);
            break;
        case queue::all :
            benchmark<queue::push_helper>(iteration,io);
            benchmark<queue::pop_helper>(iteration,io);
            benchmark<queue::push_one_helper>(iteration,io);
            benchmark<queue::mhines_bench_helper>(iteration,io);
            break;
        default:
            return mapp::MAPP_BAD_ARG;
    }
    return mapp::MAPP_OK;
}


int queue_execute(int argc, char* const argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(int error = queue_help(argc, argv, vm)) return error;
        queue_content(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return mapp::MAPP_UNKNOWN_ERROR;
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}
