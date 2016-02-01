/*
 * Neuromapp - kernel.cpp, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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
 * @file neuromapp/test/coreneuron_1.0/kernel.cpp
 *  Test on the kernel miniapp
 */


#define BOOST_TEST_MODULE KernelTest
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>

#include "coreneuron_1.0/kernel/kernel.h" // signature kernel application
#include "neuromapp/coreneuron_1.0/common/data/path.h" // this file is generated automatically
#include "coreneuron_1.0/common/data/helper.h" // common functionalities
#include "utils/error.h"

namespace bfs = ::boost::filesystem;

BOOST_AUTO_TEST_CASE(helper_solver_test){
    std::vector<std::string> command_v;
    int error(mapp::MAPP_OK);

    //no input
    command_v.push_back("coreneuron10_kernel_execute"); // dummy argument to be compliant with getopt
    error = mapp::execute(command_v,coreneuron10_kernel_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //wrong data
    command_v.clear();
    command_v.push_back("coreneuron10_kernel_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--data");
    command_v.push_back("fake and wrong");
    error = mapp::execute(command_v,coreneuron10_kernel_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //wrong argument
    command_v.clear();
    command_v.push_back("coreneuron10_kernel_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--tqrhqrhqethqhba"); // this does not exist
    error = mapp::execute(command_v,coreneuron10_kernel_execute);
    BOOST_CHECK(error==mapp::MAPP_USAGE);

    //call help
    command_v.clear();
    command_v.push_back("coreneuron10_kernel_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--help"); // help menu
    error = mapp::execute(command_v,coreneuron10_kernel_execute);
    BOOST_CHECK(error==mapp::MAPP_USAGE);

    //wrong mechanism
    command_v.clear();
    command_v.push_back("coreneuron10_kernel_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--mechanism"); // help menu
    command_v.push_back("wrong");
    error = mapp::execute(command_v,coreneuron10_kernel_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_ARG);

    //wrong function associate to mechanism
    command_v.clear();
    command_v.push_back("coreneuron10_kernel_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--function"); // help menu
    command_v.push_back("wrong");
    error = mapp::execute(command_v,coreneuron10_kernel_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_ARG);
}

BOOST_AUTO_TEST_CASE(kernels_test){
    bfs::path p(mapp::data_test());
    bool b = bfs::exists(p);
    BOOST_CHECK(b); //data ready, live or die

    std::vector<std::string> command_v;

    std::string mechanism("--mechanism");
    std::vector<std::string> vmechanism;
    vmechanism.push_back("Na");
    vmechanism.push_back("Ih");
    vmechanism.push_back("ProbAMPANMDA");

    std::string function("--function");
    std::vector<std::string> vfunction;
    vfunction.push_back("state");
    vfunction.push_back("current");

    std::string path(mapp::data_test());

    std::vector<std::string>::iterator itm = vmechanism.begin();

    int error = mapp::MAPP_OK;

    for(;itm != vmechanism.end();++itm){
        std::vector<std::string>::iterator itf = vfunction.begin();
        for(;itf != vfunction.end();++itf){
            command_v.push_back("coreneuron10_kernel_execute"); // dummy argument to be compliant with getopt
            command_v.push_back(mechanism);
            command_v.push_back(*itm);
            command_v.push_back(function);
            command_v.push_back(*itf);
            command_v.push_back("--data");
            command_v.push_back(path);
            command_v.push_back("--name"); // name for the storage class
            command_v.push_back("kernel_test"+command_v[2]);
            error = mapp::execute(command_v,coreneuron10_kernel_execute);
            BOOST_CHECK(error==mapp::MAPP_OK);
            command_v.clear();
        }
    }
}

BOOST_AUTO_TEST_CASE(kernels_reference_solution_test){
    bfs::path p(mapp::data_test());
    bool b = bfs::exists(p);
    BOOST_CHECK(b); //data ready, live or die

    std::string name("coreneuron_1.0_kernel_data");
    std::string path(mapp::data_test());

    std::string mechanisms[3] = {"Na","Ih","ProbAMPANMDA"};
    std::string functors[2] = {"state","current"};

    std::string mechanism("mechanism");

    std::vector<std::string> command_v;
    command_v.push_back("coreneuron10_kernel_execute");
    command_v.push_back("--mechanism");
    command_v.push_back(mechanism);
    command_v.push_back("--function");
    command_v.push_back("functor");
    command_v.push_back("--data");
    command_v.push_back(path);
    command_v.push_back("--name");
    command_v.push_back("dummy");

    int error = mapp::MAPP_OK;

    for(size_t i(0); i < 3 ;++i){
        // every run must be independant, so different name for the storage map
        command_v[0] = name;
        command_v[2] = mechanisms[i];
        command_v[4] = functors[0];
        command_v[8] = "internal_storage_name_"+mechanisms[i];

        //state first
        error = mapp::execute(command_v,coreneuron10_kernel_execute);
        BOOST_CHECK(error==mapp::MAPP_OK);
        //current second
        command_v[4] = functors[1];
        error = mapp::execute(command_v,coreneuron10_kernel_execute);
        BOOST_CHECK(error==mapp::MAPP_OK);
        mapp::helper_check(command_v[8],mechanisms[i],path);
    }
}
