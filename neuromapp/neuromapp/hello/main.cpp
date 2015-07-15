#include <iostream>
#include <string>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <boost/program_options.hpp>

#include "hello/hello.h"

namespace po = boost::program_options;

/** help function provide the help for the user */
int help(int argc, char* argv[], po::variables_map& vm){
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("name", po::value<std::string>()->default_value("world"), "who ?")
    ("numthread", po::value<int>()->default_value(1), "number of OMP thread");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

#ifdef _OPENMP
    omp_set_num_threads(vm["numthread"].as<int>());
#endif

    if (vm.count("help")){
        std::cout << desc;
        return 1;
    }
    return 0;
}

/** execute the content of the miniapp */
void content(po::variables_map const& vm){
    #pragma omp parallel
    #pragma omp critical
    {
#ifdef _OPENMP
        std::cout << "Hello " << vm["name"].as<std::string>()
                  << ", total thread: " << vm["numthread"].as<int>()
                  << ", thread id: " << omp_get_thread_num() << "\n";
#else
        std::cout << "Hello " << vm["name"].as<std::string>() << "\n";
#endif
    }
}

int hello_execute(int argc, char* argv[]){
    try {
        po::variables_map vm; // it contains everything
        if(help(argc, argv, vm)) return 1;
        content(vm); // execute the miniapp
    }
    catch(std::exception& e){
        std::cout << e.what() << "\n";
        return 1;
    }
    return 0; // 0 ok, 1 not ok
}
