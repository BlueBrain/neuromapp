#define BOOST_TEST_MODULE KernelTest
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>

extern "C" {
#include "utils/storage/storage.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
}

#include "coreneuron_1.0/cstep/cstep.h" // signature kernel application
#include "test/coreneuron_1.0/path.h" // this file is generated automatically
#include "test/coreneuron_1.0/helper.h" // common functionalities
#include "utils/error.h"

namespace bfs = ::boost::filesystem;

BOOST_AUTO_TEST_CASE(helper_solver_test){
    std::vector<std::string> command_v;
    int error(mapp::MAPP_OK);

    //no data
    command_v.push_back("cstep_solver_execute"); // dummy argument to be compliant with getopt
    error = mapp::execute(command_v,coreneuron10_cstep_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //wrong data
    command_v.clear();
    command_v.push_back("cstep_solver_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--data");
    command_v.push_back("fake and wrong");
    error = mapp::execute(command_v,coreneuron10_cstep_execute);
    BOOST_CHECK(error==mapp::MAPP_BAD_DATA);

    //wrong argument
    command_v.clear();
    command_v.push_back("cstep_solver_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--qrhqrhqethqhba"); // this does not exist
    error = mapp::execute(command_v,coreneuron10_cstep_execute);
    BOOST_CHECK(error==mapp::MAPP_USAGE);

    //helper
    command_v.clear();
    command_v.push_back("coreneuron10_solver_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--help"); // help menu
    error = mapp::execute(command_v,coreneuron10_cstep_execute);
    BOOST_CHECK(error==mapp::MAPP_USAGE);
}

BOOST_AUTO_TEST_CASE(cstep_reference_solution_test){
    bfs::path p(mapp::data_test());
    bool b = bfs::exists(p);
    BOOST_CHECK(b); //data ready, live or die

    //preparing the command line
    std::vector<std::string> command_v;
    command_v.push_back("coreneuron10_cstep");
    command_v.push_back("--data");
    command_v.push_back(mapp::data_test());
    command_v.push_back("--name");
    command_v.push_back("coreneuron10_cstep");

    int num = mapp::execute(command_v,coreneuron10_cstep_execute);
    BOOST_CHECK(num==0);
    mapp::helper_check(command_v[4],"cstep",mapp::data_test());
}
