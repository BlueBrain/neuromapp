#define BOOST_TEST_MODULE SolverTest
#include <vector>
#include <limits>
#include <cmath>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "coreneuron_1.0/solver/solver.h" // signature kernel application
#include "coreneuron_1.0/solver/hines.h" // to call the solver library's API directly
#include "coreneuron_1.0/test/path.h" // this file is generated automatically
#include "coreneuron_1.0/test/helper.hpp" // common functionalities

namespace bfs = ::boost::filesystem;

BOOST_AUTO_TEST_CASE(solver_test){
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

