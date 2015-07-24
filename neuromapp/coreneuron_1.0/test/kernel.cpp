#define BOOST_TEST_MODULE KernelTest
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>

#include "coreneuron_1.0/kernel/kernel.h" // signature kernel application
#include "coreneuron_1.0/test/path.h" // this file is generated automatically
#include "coreneuron_1.0/test/helper.hpp" // common functionalities

namespace bfs = ::boost::filesystem;

BOOST_AUTO_TEST_CASE(kernel_test){
    bfs::path p(mapp::path_unzip());
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

    std::string path(mapp::path_unzip());

    std::vector<std::string>::iterator itm = vmechanism.begin();

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
            int num = mapp::execute(command_v,coreneuron10_kernel_execute);
            BOOST_CHECK(num==0);
            command_v.clear();
        }
    }
}
