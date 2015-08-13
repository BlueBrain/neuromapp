/*
 * Neuromapp - solver.cpp, Copyright (c), 2015,
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
 * @file neuromapp/test/coreneuron_1.0/solver.cpp
 *  Test on the solver miniapp
 */


#define BOOST_TEST_MODULE SolverTest
#include <vector>
#include <limits>
#include <cmath>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "coreneuron_1.0/solver/solver.h" // signature kernel application
#include "coreneuron_1.0/solver/hines.h" // to call the solver library's API directly

#include "test/coreneuron_1.0/path.h" // this file is generated automatically
#include "test/coreneuron_1.0/helper.h" // common functionalities
#include "utils/error.h"

namespace bfs = ::boost::filesystem;

BOOST_AUTO_TEST_CASE(helper_solver_test){
    std::vector<std::string> command_v;
    std::string path(mapp::data_test());
    int error(mapp::MAPP_OK);

    //no input
    command_v.push_back("coreneuron10_solver_execute"); // dummy argument to be compliant with getopt
    error = mapp::execute(command_v,coreneuron10_solver_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //no relevant data
    command_v.clear();
    command_v.push_back("coreneuron10_solver_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--data");
    command_v.push_back("fake and wrong");
    error = mapp::execute(command_v,coreneuron10_solver_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //useless argument
    command_v.clear();
    command_v.push_back("coreneuron10_solver_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--tqrhqrhqethqhba"); // this does not exist
    error = mapp::execute(command_v,coreneuron10_solver_execute);
    BOOST_CHECK(error==mapp::MAPP_USAGE);

    //asking for helper
    command_v.clear();
    command_v.push_back("coreneuron10_solver_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--help"); // help menu
    error = mapp::execute(command_v,coreneuron10_solver_execute);
    BOOST_CHECK(error==mapp::MAPP_USAGE);
}

BOOST_AUTO_TEST_CASE(solver_test){
    bfs::path p(mapp::data_test());
    bool b = bfs::exists(p);
    BOOST_CHECK(b); //data ready, live or die

    std::string path(mapp::data_test());
    std::vector<std::string> command_v;
    command_v.push_back("coreneuron10_solver_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--data");
    command_v.push_back(path);

    int num = mapp::execute(command_v,coreneuron10_solver_execute);
    BOOST_CHECK(num==0);
}

BOOST_AUTO_TEST_CASE(simple_matrix_solver_test){
    //smallest matrix we can represent is a 3x3
    NrnThread nt;

    nt.ncell=1;
    nt.end=3;

    double actual_d_data[3]= { 1., 1., 1. };
    double actual_a_data[3]= { 0., -1., 0.5 };
    double actual_b_data[3]= { 0., 0.5, -1.0 };
    double actual_rhs_data[3]= { 0., 2., 0. };
    int _v_parent_index_data[3] = { 0, 0, 1};

    nt._actual_d = actual_d_data;
    nt._actual_a = actual_a_data;
    nt._actual_b = actual_b_data;
    nt._actual_rhs = actual_rhs_data;
    nt._v_parent_index = _v_parent_index_data;

    nrn_solve_minimal(&nt);

    //compute error bounds from condition number and machine epsilon
    double gamma = std::numeric_limits<double>::epsilon();
    double cond_max = 4.0; // condition number in max norm, computed with matlab
    for(int i=0;i<nt.end;++i) {
        BOOST_CHECK_CLOSE(nt._actual_rhs[i], 1.0, 0.000001 );
        BOOST_CHECK( std::abs(nt._actual_rhs[i] - 1.0) <= 2.0*gamma*cond_max/(1.0 - gamma*cond_max) );
    }

}

