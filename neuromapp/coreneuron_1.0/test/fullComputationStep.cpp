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

#include "coreneuron_1.0/fullComputationStep/fullComputationStep.h" // signature kernel application
#include "coreneuron_1.0/test/path.h" // this file is generated automatically
#include "coreneuron_1.0/test/helper.h" // common functionalities

namespace bfs = ::boost::filesystem;

BOOST_AUTO_TEST_CASE(fullComputationalStep_reference_solution_test){
    bfs::path p(mapp::path_unzip());
    bool b = bfs::exists(p);
    BOOST_CHECK(b); //data ready, live or die

    //preparing the command line
    std::vector<std::string> command_v;
    command_v.push_back("coreneuron10_fullComputationalStep");
    command_v.push_back("--data");
    command_v.push_back(mapp::path_unzip());
    command_v.push_back("--name");
    command_v.push_back("coreneuron10_fullComputationalStep");

    int num = mapp::execute(command_v,coreneuron10_fullComputationStep_execute);
    BOOST_CHECK(num==0);
    mapp::helper_check(command_v[4],"fullComputationalStep",mapp::path_unzip());
}
