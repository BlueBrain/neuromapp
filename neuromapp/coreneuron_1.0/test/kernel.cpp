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
            command_v.push_back("--name"); // name for the storage class
            command_v.push_back("kernel_test"+command_v[2]);
            int num = mapp::execute(command_v,coreneuron10_kernel_execute);
            BOOST_CHECK(num==0);
            command_v.clear();
        }
    }

}

void helper_check(std::string const& name, std::string const& mechanism, std::string const& path){
    //extract what we need d and rhs
    NrnThread * nt = (NrnThread *) storage_get (name.c_str(),
                                                make_nrnthread, (void*)path.c_str(), dealloc_nrnthread);

    int size = get_end(nt);
    double* rhs = get_rhs(nt); // compute solutiom rhs
    double* d = get_d(nt); // compute solutiom d

    double* ref_rhs = new double[size];
    double* ref_d = new double[size];

    std::string data = "rhs_d_ref/rhs_d_"+mechanism;

//    std::ofstream outfile(data);
//    for(int i=0; i < size;++i)
//        outfile << " "  << d[i] << " " <<  rhs[i] << "\n";

    std::ifstream infile(data.c_str());

    for(int i=0; i < size;++i)
        infile >> ref_d[i] >> ref_rhs[i];

    infile.close();

    for(int i=0; i < size;++i){
        BOOST_CHECK_CLOSE( ref_d[i], d[i], 0.01 );
        BOOST_CHECK_CLOSE( ref_rhs[i], rhs[i], 0.01 );
    }

    delete [] ref_rhs;
    delete [] ref_d;
}

BOOST_AUTO_TEST_CASE(all_test){
    bfs::path p(mapp::path_unzip());
    bool b = bfs::exists(p);
    BOOST_CHECK(b); //data ready, live or die

    std::string name("coreneuron_1.0_kernel_data");
    std::string path(mapp::path_unzip());

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

    for(size_t i(0); i < 3 ;++i){
        // every run must be independant, so different name for the storage map
        command_v[0] = name;
        command_v[2] = mechanisms[i];
        command_v[4] = functors[0];
        command_v[8] = "internal_storage_name_"+mechanisms[i];

        //state first
        int num = mapp::execute(command_v,coreneuron10_kernel_execute);
        BOOST_CHECK(num==0);
        //current second
        command_v[4] = functors[1];
        num = mapp::execute(command_v,coreneuron10_kernel_execute);
        BOOST_CHECK(num==0);
        helper_check(command_v[8],mechanisms[i],path);
    }
}
