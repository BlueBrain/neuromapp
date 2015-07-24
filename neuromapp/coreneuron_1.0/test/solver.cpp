#define BOOST_TEST_MODULE SolverTest
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "coreneuron_1.0/solver/solver.h" // signature kernel application
#include "coreneuron_1.0/test/path.h" // this file is generated automatically
#include "coreneuron_1.0/test/helper.hpp" // common functionalities

namespace bfs = ::boost::filesystem;

BOOST_AUTO_TEST_CASE(kernel_test){
    bfs::path p(mapp::path_unzip());
    bool b = bfs::exists(p);
    BOOST_CHECK(b); //data ready, live or die

    std::string path(mapp::path_unzip());
    std::vector<std::string> command_v;
    command_v.push_back("coreneuron10_solver_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--data");
    command_v.push_back(path);

    int num = mapp::execute(command_v,coreneuron10_solver_execute);
    BOOST_CHECK(num==0);
}
